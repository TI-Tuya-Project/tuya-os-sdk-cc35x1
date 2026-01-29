/**
 * @file tkl_wakeup.c
 * @brief Tuya TKL wakeup adapter for TI (CC35xx) using TI GPIO driver
 */

// --- BEGIN: user defines and implements ---
#include "tkl_wakeup.h"
#include "tuya_error_code.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ti/drivers/GPIO.h>

// Dependency Injection
#include "tkl_board_config.h"
// --- END: user defines and implements ---

static bool s_gpio_inited = false;

typedef struct {
    bool active;
    TUYA_GPIO_NUM_E tuya_gpio;
    int ti_index;
    TUYA_GPIO_WAKE_TYPE_E wake_type;
} wake_gpio_ctx_t;

static wake_gpio_ctx_t g_wake_gpio = {0};

static void ti_wakeup_gpio_isr(uint_least8_t index);

static inline uint32_t map_tuya_wake_to_ti_int(TUYA_GPIO_WAKE_TYPE_E t)
{
    switch (t) {
        case TUYA_GPIO_WAKEUP_RISE: return GPIO_CFG_IN_INT_RISING;
        case TUYA_GPIO_WAKEUP_FALL: return GPIO_CFG_IN_INT_FALLING;
        case TUYA_GPIO_WAKEUP_HIGH: return GPIO_CFG_IN_INT_RISING; // Approx
        case TUYA_GPIO_WAKEUP_LOW:  return GPIO_CFG_IN_INT_FALLING; // Approx
        default:                    return GPIO_CFG_IN_INT_FALLING;
    }
}

static OPERATE_RET wake_gpio_enable(int ti_index, TUYA_GPIO_WAKE_TYPE_E wake_type)
{
    if (ti_index < 0) return OPRT_INVALID_PARM;

    if (!s_gpio_inited) {
        GPIO_init();
        s_gpio_inited = true;
    }

    uint32_t cfg = GPIO_CFG_IN_PU | map_tuya_wake_to_ti_int(wake_type);

    GPIO_setConfig((uint_least8_t)ti_index, cfg);
    GPIO_setCallback((uint_least8_t)ti_index, ti_wakeup_gpio_isr);
    GPIO_enableInt((uint_least8_t)ti_index);

    return OPRT_OK;
}

static OPERATE_RET wake_gpio_disable(int ti_index)
{
    if (ti_index < 0) return OPRT_INVALID_PARM;
    if (!s_gpio_inited) return OPRT_OK;

    GPIO_disableInt((uint_least8_t)ti_index);
    GPIO_setCallback((uint_least8_t)ti_index, NULL);
    return OPRT_OK;
}

static void ti_wakeup_gpio_isr(uint_least8_t index)
{
    (void)index; // Hook for future use
}

/* ============ Tuya APIs ============ */

OPERATE_RET tkl_wakeup_source_set(const TUYA_WAKEUP_SOURCE_BASE_CFG_T *param)
{
    if (param == NULL) return OPRT_INVALID_PARM;

    if (param->source == TUYA_WAKEUP_SOURCE_GPIO) {

        const TUYA_WAKEUP_SOURCE_GPIO_T *g = &param->wakeup_para.gpio_param;

        // [DYNAMIC LOOKUP]
        int16_t ti_index = tkl_hw_get_gpio_index(g->gpio_num);
        
        if (ti_index < 0) return OPRT_NOT_SUPPORTED;

        if (g_wake_gpio.active) {
            wake_gpio_disable(g_wake_gpio.ti_index);
            memset(&g_wake_gpio, 0, sizeof(g_wake_gpio));
        }

        OPERATE_RET rc = wake_gpio_enable(ti_index, g->level);
        if (rc != OPRT_OK) return rc;

        g_wake_gpio.active = true;
        g_wake_gpio.tuya_gpio = g->gpio_num;
        g_wake_gpio.ti_index = ti_index;
        g_wake_gpio.wake_type = g->level;

        return OPRT_OK;
    }

    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wakeup_source_clear(const TUYA_WAKEUP_SOURCE_BASE_CFG_T *param)
{
    if (param == NULL) {
        if (g_wake_gpio.active) {
            wake_gpio_disable(g_wake_gpio.ti_index);
            memset(&g_wake_gpio, 0, sizeof(g_wake_gpio));
        }
        return OPRT_OK;
    }

    if (param->source == TUYA_WAKEUP_SOURCE_GPIO) {
        if (g_wake_gpio.active) {
            wake_gpio_disable(g_wake_gpio.ti_index);
            memset(&g_wake_gpio, 0, sizeof(g_wake_gpio));
        }
        return OPRT_OK;
    }

    return OPRT_NOT_SUPPORTED;
}