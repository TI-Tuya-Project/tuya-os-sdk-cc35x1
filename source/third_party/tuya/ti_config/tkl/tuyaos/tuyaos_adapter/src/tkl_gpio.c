// --- BEGIN: user defines and implements ---
#include "tkl_gpio.h"
#include "tuya_error_code.h"
#include <ti/drivers/gpio/GPIOWFF3.h>
#include <stdbool.h>
#include <string.h>

// Dependency Injection Header
#include "tkl_board_config.h"

// Max safe HW index for TI chips (CC32xx < 64)
#define TI_HW_GPIO_MAX_INDICES  64 

// Callback storage
typedef struct {
    TUYA_GPIO_IRQ_T callback; 
} pin_cb_store_t;

// Storage for Callbacks only. The HW mapping is now in tkl_board.c
static pin_cb_store_t sg_pin_callbacks[TKL_HW_MAX_GPIO_PINS];

// Reverse Lookup: TI Index -> Tuya Pin ID (Needed for ISR bridge)
static int8_t sg_ti_to_tuya_lookup[TI_HW_GPIO_MAX_INDICES];
static bool sg_is_lookup_init = false;

// Helper: Init lookup table
static void ensure_lookup_table_init(void) {
    if (!sg_is_lookup_init) {
        memset(sg_ti_to_tuya_lookup, -1, sizeof(sg_ti_to_tuya_lookup));
        memset(sg_pin_callbacks, 0, sizeof(sg_pin_callbacks));
        sg_is_lookup_init = true;
    }
}

// ISR Bridge: TI calls this with HW Index -> We find Tuya ID -> Call Tuya CB
static void ti_gpio_callback_bridge(uint_least8_t ti_index)
{
    if (ti_index >= TI_HW_GPIO_MAX_INDICES) return;

    int8_t tuya_pin_id = sg_ti_to_tuya_lookup[ti_index];

    if (tuya_pin_id >= 0 && tuya_pin_id < TKL_HW_MAX_GPIO_PINS) {
        if (sg_pin_callbacks[tuya_pin_id].callback.cb != NULL) {
            sg_pin_callbacks[tuya_pin_id].callback.cb(sg_pin_callbacks[tuya_pin_id].callback.arg);
        }
    }
}
// --- END: user defines and implements ---

/* @brief gpio init
 */
OPERATE_RET tkl_gpio_init(TUYA_GPIO_NUM_E pin_id, const TUYA_GPIO_BASE_CFG_T *cfg)
{
    if (pin_id >= TKL_HW_MAX_GPIO_PINS) return OPRT_INVALID_PARM;
        ensure_lookup_table_init();

    // [DYNAMIC LOOKUP] Get the real TI hardware index
    int16_t ti_index = tkl_hw_get_gpio_index(pin_id);

    if (ti_index < 0) {
        return OPRT_NOT_SUPPORTED; // Pin not mapped by Application
    }

    // Update reverse lookup for ISRs
    if (ti_index < TI_HW_GPIO_MAX_INDICES) {
        sg_ti_to_tuya_lookup[ti_index] = (int8_t)pin_id;
    }

    uint32_t config_ti = 0;

    // Map Configuration (Input/Output/Pull)
    switch (cfg->direct) {
        case TUYA_GPIO_INPUT:
            config_ti = GPIO_CFG_INPUT;
            switch (cfg->mode) { 
                case TUYA_GPIO_PULLUP:   config_ti |= GPIO_CFG_IN_PU; break;
                case TUYA_GPIO_PULLDOWN: config_ti |= GPIO_CFG_IN_PD; break;
                default:                 config_ti |= GPIO_CFG_IN_NOPULL; break;
            }
            break;

        case TUYA_GPIO_OUTPUT:
            config_ti = GPIO_CFG_OUTPUT;
            switch (cfg->level) {
                case TUYA_GPIO_LEVEL_HIGH: config_ti |= GPIO_CFG_OUT_HIGH; break;
                default:                   config_ti |= GPIO_CFG_OUT_LOW; break;
            }
            break;

        default:
            return OPRT_NOT_SUPPORTED;
    }

    // Apply to TI Driver
    if (GPIO_setConfig(ti_index, config_ti) != GPIO_STATUS_SUCCESS) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
}

/**
 * @brief gpio deinit
 */
OPERATE_RET tkl_gpio_deinit(TUYA_GPIO_NUM_E pin_id)
{
    // --- BEGIN: user implements ---
    if (pin_id >= TKL_HW_MAX_GPIO_PINS) return OPRT_INVALID_PARM;

    int16_t ti_index = tkl_hw_get_gpio_index(pin_id);
    if (ti_index < 0) return OPRT_NOT_SUPPORTED;

    GPIO_disableInt(ti_index);
    GPIO_setCallback(ti_index, NULL);
    GPIO_setConfig(ti_index, GPIO_CFG_INPUT | GPIO_CFG_IN_NOPULL);
    
    // Clear callback
    sg_pin_callbacks[pin_id].callback.cb = NULL;
    
    // Clear reverse lookup
    if (ti_index < TI_HW_GPIO_MAX_INDICES) {
        sg_ti_to_tuya_lookup[ti_index] = -1;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio write
 */
OPERATE_RET tkl_gpio_write(TUYA_GPIO_NUM_E pin_id, TUYA_GPIO_LEVEL_E level)
{
    // --- BEGIN: user implements ---
    if (pin_id >= TKL_HW_MAX_GPIO_PINS) return OPRT_INVALID_PARM;
    
    int16_t ti_index = tkl_hw_get_gpio_index(pin_id);
    if (ti_index < 0) return OPRT_NOT_SUPPORTED;

    GPIO_write(ti_index, (unsigned int)level);
    return OPRT_OK;    
    // --- END: user implements ---
}

/**
 * @brief gpio read
 */
OPERATE_RET tkl_gpio_read(TUYA_GPIO_NUM_E pin_id, TUYA_GPIO_LEVEL_E *level)
{
    // --- BEGIN: user implements ---
    if (pin_id >= TKL_HW_MAX_GPIO_PINS || level == NULL) return OPRT_INVALID_PARM;

    int16_t ti_index = tkl_hw_get_gpio_index(pin_id);
    if (ti_index < 0) return OPRT_NOT_SUPPORTED;

    *level = (TUYA_GPIO_LEVEL_E)GPIO_read(ti_index);
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio irq init
 */
OPERATE_RET tkl_gpio_irq_init(TUYA_GPIO_NUM_E pin_id, const TUYA_GPIO_IRQ_T *cfg)
{
    // --- BEGIN: user implements ---
    if (pin_id >= TKL_HW_MAX_GPIO_PINS || cfg == NULL) return OPRT_INVALID_PARM;
    ensure_lookup_table_init();

    int16_t ti_index = tkl_hw_get_gpio_index(pin_id);
    if (ti_index < 0) return OPRT_NOT_SUPPORTED;

    // Update reverse lookup
    if (ti_index < TI_HW_GPIO_MAX_INDICES) {
        sg_ti_to_tuya_lookup[ti_index] = (int8_t)pin_id;
    }

    // Store CB
    sg_pin_callbacks[pin_id].callback = *cfg;

    uint32_t ti_irq_mode = 0;
    switch(cfg->mode){
        case TUYA_GPIO_IRQ_RISE: ti_irq_mode = GPIO_CFG_IN_INT_RISING; break;
        case TUYA_GPIO_IRQ_FALL: ti_irq_mode = GPIO_CFG_IN_INT_FALLING; break;
        default:                 return OPRT_NOT_SUPPORTED;        
    }

    GPIO_setInterruptConfig(ti_index, ti_irq_mode);
    GPIO_setCallback(ti_index, ti_gpio_callback_bridge);

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio irq enable
 */
OPERATE_RET tkl_gpio_irq_enable(TUYA_GPIO_NUM_E pin_id)
{
    // --- BEGIN: user implements ---
    if (pin_id >= TKL_HW_MAX_GPIO_PINS) return OPRT_INVALID_PARM;

    int16_t ti_index = tkl_hw_get_gpio_index(pin_id);
    if (ti_index < 0) return OPRT_NOT_SUPPORTED;

    GPIO_enableInt(ti_index);
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio irq disable
 */
OPERATE_RET tkl_gpio_irq_disable(TUYA_GPIO_NUM_E pin_id)
{
    // --- BEGIN: user implements ---
    if (pin_id >= TKL_HW_MAX_GPIO_PINS) return OPRT_INVALID_PARM;

    int16_t ti_index = tkl_hw_get_gpio_index(pin_id);
    if (ti_index < 0) return OPRT_NOT_SUPPORTED;

    GPIO_disableInt(ti_index);
    return OPRT_OK;
    // --- END: user implements ---
}