// --- BEGIN: user defines and implements ---
#include "tkl_gpio.h"
#include "tuya_error_code.h"
#include <ti_drivers_config.h> // Program Will compile at the time of the app generation
#include <ti/drivers/gpio/GPIOWFF3.h>
#include <stdbool.h>
#include <string.h> // Required for memset

// [OPTIMIZATION] Define a safe upper bound for TI GPIO Indices (CC32xx usually has < 64)
// This prevents magic numbers in array declarations.
#define TI_HW_GPIO_MAX_INDICES  64 

// [NO MAGIC NUMBERS] Macro to calculate the number of configured buttons automatically
#define TUYA_GPIO_MAP_SIZE      (sizeof(cb_gpio_map) / sizeof(cb_gpio_map[0]))

typedef uint8_t gpio_id_t;

typedef struct {
    gpio_id_t gpio_id;
    TUYA_GPIO_IRQ_T callback; 
} pin_dev_map_t;

// [USER CONFIG] Add as many buttons as needed here. Logic adapts automatically.
static pin_dev_map_t cb_gpio_map[] = {
    {CONFIG_GPIO_BUTTON_0, {0}}, // GPIO_0 changed to TI CONFIG name for safety
    {CONFIG_GPIO_BUTTON_1, {0}},
    {CONFIG_GPIO_LED_0,    {0}},
    // Add more entries here...
};

// [OPTIMIZATION] Reverse Lookup Table: Maps TI Index -> Tuya Pin ID
// Initialize to -1 to indicate "Unmapped"
static int8_t sg_ti_to_tuya_lookup[TI_HW_GPIO_MAX_INDICES];
static bool sg_is_lookup_init = false;

// [OPTIMIZATION] O(1) Constant Time Bridge
// No looping required. Instantly finds the Tuya ID from the TI Index.
static void ti_gpio_callback_bridge(uint_least8_t ti_index)
{
    // Safety check for HW index bounds
    if (ti_index >= TI_HW_GPIO_MAX_INDICES) return;

    // Fast lookup
    int8_t tuya_pin_id = sg_ti_to_tuya_lookup[ti_index];

    // If valid mapping exists (-1 means invalid)
    if (tuya_pin_id >= 0 && tuya_pin_id < TUYA_GPIO_MAP_SIZE) {
        if (cb_gpio_map[tuya_pin_id].callback.cb != NULL) {
            cb_gpio_map[tuya_pin_id].callback.cb(cb_gpio_map[tuya_pin_id].callback.args);
        }
    }
}

// Helper: Initializes the lookup table once
static void ensure_lookup_table_init(void) {
    if (!sg_is_lookup_init) {
        memset(sg_ti_to_tuya_lookup, -1, sizeof(sg_ti_to_tuya_lookup));
        sg_is_lookup_init = true;
    }
}

// [MACRO] Fixed variable name from 'index' to 'req_id' to match usage
#define CHECK_BOUNDED(req_id) \
    do { \
        if ((req_id) >= TUYA_GPIO_MAP_SIZE) \
            return OPRT_INVALID_PARM; \
    } while(0)

// --- END: user defines and implements ---

 * @brief gpio init
 */
OPERATE_RET tkl_gpio_init(TUYA_GPIO_NUM_E pin_id, const TUYA_GPIO_BASE_CFG_T *cfg)
{
    // --- BEGIN: user implements ---
    CHECK_BOUNDED(pin_id);
    ensure_lookup_table_init();

    uint32_t config_ti = 0;
    uint8_t gpio_index = cb_gpio_map[pin_id].gpio_id;

    // [OPTIMIZATION] Update reverse lookup table for this pin
    if (gpio_index < TI_HW_GPIO_MAX_INDICES) {
        sg_ti_to_tuya_lookup[gpio_index] = (int8_t)pin_id;
    }

    // Outer Switch: Direction (Input vs Output)
    switch (cfg->direct) {
        case TUYA_GPIO_INPUT:
            config_ti = GPIO_CFG_INPUT;
            
            // Inner Switch: Pull Mode
            // Refers to the Request part
            switch (cfg->mode) { 
                case TUYA_GPIO_PULLUP:              // SDK wants the pin held at Logic high(1) 
                    config_ti |= GPIO_CFG_IN_PU;    // Activate the internal Pull-up resistor connected to Vcc
                    break;
                case TUYA_GPIO_PULLDOWN:            // SDK wants the pin held at Logic low(0) 
                    config_ti |= GPIO_CFG_IN_PD;    // Activates the internal Pull-Down resistor connected to the ground
                    break;
                case TUYA_GPIO_FLOATING:            // SDK wants the pin in a high impedance state (no internal resistors active)
                default:
                    config_ti |= GPIO_CFG_IN_NOPULL; // Disables all internal resistors, leaving the pin "floating"
                    break;
            }
            break;

        case TUYA_GPIO_OUTPUT:
            config_ti = GPIO_CFG_OUTPUT;
            
            // Inner Switch: Initial Level
            switch (cfg->level) {
                case TUYA_GPIO_LEVEL_HIGH:  
                    config_ti |= GPIO_CFG_OUT_HIGH; // Initial Voltage level set to a logical "1"(Hv -> ground/3.3V)
                    break;
                case TUYA_GPIO_LEVEL_LOW: 
                default:
                    config_ti |= GPIO_CFG_OUT_LOW;  // Initial Voltage level set to a logical "0"(Lv -> ground/0V)
                    break;
            }
            break;

        default:
            return OPRT_NOT_SUPPORTED;
    }

    // Apply the configuration to the TI hardware
    if (GPIO_setConfig(gpio_index, config_ti) != GPIO_STATUS_SUCCESS) {
        return OPRT_COM_ERROR; // Status Was unsuccessful
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio deinit
 */
OPERATE_RET tkl_gpio_deinit(TUYA_GPIO_NUM_E pin_id)
{
    // --- BEGIN: user implements ---
    CHECK_BOUNDED(pin_id);

    uint8_t gpio_index = cb_gpio_map[pin_id].gpio_id;
    
    GPIO_disableInt(gpio_index);
    GPIO_setCallback(gpio_index, NULL);
    GPIO_setConfig(gpio_index, GPIO_CFG_INPUT | GPIO_CFG_IN_NOPULL);
    
    // Clear internal state
    cb_gpio_map[pin_id].callback.cb = NULL;
    
    // [OPTIMIZATION] Clear the reverse lookup entry
    if (gpio_index < TI_HW_GPIO_MAX_INDICES) {
        sg_ti_to_tuya_lookup[gpio_index] = -1;
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
    CHECK_BOUNDED(pin_id);
    uint8_t gpio_index = cb_gpio_map[pin_id].gpio_id;
    GPIO_write(gpio_index, (unsigned int)level);
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio read
 */
OPERATE_RET tkl_gpio_read(TUYA_GPIO_NUM_E pin_id, TUYA_GPIO_LEVEL_E *level)
{
    // --- BEGIN: user implements ---
    CHECK_BOUNDED(pin_id);
    if (level == NULL) return OPRT_INVALID_PARM;

    uint8_t gpio_index = cb_gpio_map[pin_id].gpio_id;
    *level = (TUYA_GPIO_LEVEL_E)GPIO_read(gpio_index); // TI's read returns 1 or 0
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio irq init
 */
OPERATE_RET tkl_gpio_irq_init(TUYA_GPIO_NUM_E pin_id, const TUYA_GPIO_IRQ_T *cfg)
{
    // --- BEGIN: user implements ---
    CHECK_BOUNDED(pin_id);
    ensure_lookup_table_init();

    uint_least8_t gpio_index = cb_gpio_map[pin_id].gpio_id;
    uint32_t ti_irq_mode = 0;

    // [OPTIMIZATION] Update reverse lookup table (just in case init wasn't called)
    if (gpio_index < TI_HW_GPIO_MAX_INDICES) {
        sg_ti_to_tuya_lookup[gpio_index] = (int8_t)pin_id;
    }

    // 1. Save the Tuya callback into our map
    // [FIX] Use pin_id here, not gpio_index!
    cb_gpio_map[pin_id].callback = *cfg;

    // 2. Map Tuya Trigger Mode to TI trigger mode
    switch(cfg->mode){
        case TUYA_GPIO_IRQ_RISE:
            ti_irq_mode = GPIO_CFG_IN_INT_RISING;
            break;
        case TUYA_GPIO_IRQ_FALL:
            ti_irq_mode = GPIO_CFG_IN_INT_FALLING;
            break;
        default:
            return OPRT_NOT_SUPPORTED;        
    }

    // 3. Configure TI Hardware but DO NOT enable yet (per Tuya note)
    GPIO_setInterruptConfig(gpio_index, ti_irq_mode);
    GPIO_setCallback(gpio_index, ti_gpio_callback_bridge);

    return OPRT_OK; // [FIX] Changed from NOT_SUPPORTED to OK
    // --- END: user implements ---
}

/**
 * @brief gpio irq enable
 */
OPERATE_RET tkl_gpio_irq_enable(TUYA_GPIO_NUM_E pin_id)
{
    // --- BEGIN: user implements ---
    CHECK_BOUNDED(pin_id);
    GPIO_enableInt(cb_gpio_map[pin_id].gpio_id);
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio irq disable
 */
OPERATE_RET tkl_gpio_irq_disable(TUYA_GPIO_NUM_E pin_id)
{
    // --- BEGIN: user implements ---
    CHECK_BOUNDED(pin_id); // [FIX] Macro var name matches passed arg
    
    // [FIX] Typo fix: cd_gpio_map -> cb_gpio_map
    GPIO_disableInt(cb_gpio_map[pin_id].gpio_id);
    
    return OPRT_OK;
    // --- END: user implements ---
}