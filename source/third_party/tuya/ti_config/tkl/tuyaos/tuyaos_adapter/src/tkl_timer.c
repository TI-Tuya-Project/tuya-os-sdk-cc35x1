/**
 * @file tkl_timer.c
 * @brief Tuya Kernel Layer - Timer Implementation
 */

// --- BEGIN: user defines and implements ---
#include "tkl_timer.h"
#include "tuya_error_code.h"
#include <ti/drivers/Timer.h>
#include <stddef.h>

// Dependency Injection
#include "tkl_board_config.h"

// Define max timers supported by the board config
// Note: We need a new macro in tkl_board_config.h for TKL_HW_MAX_TIMERS if not present,
// but for now we can assume 4 or add it. Let's assume 4.
#define MAX_TUYA_TIMERS 4

// Context structure
typedef struct {
    Timer_Handle handle;
    TUYA_TIMER_ISR_CB cb;
    void *args;
    uint32_t current_period_us;
} tuya_timer_ctx_t;

static tuya_timer_ctx_t g_timer_ctx[MAX_TUYA_TIMERS] = {0};

/* TI Timer Callback Wrapper */
static void _ti_timer_callback_fxn(Timer_Handle handle, int_fast16_t status)
{
    for (int i = 0; i < MAX_TUYA_TIMERS; i++) {
        if (g_timer_ctx[i].handle == handle) {
            if (g_timer_ctx[i].cb != NULL) {
                g_timer_ctx[i].cb(g_timer_ctx[i].args);
            }
            break;
        }
    }
}
// --- END: user defines and implements ---

OPERATE_RET tkl_timer_init(TUYA_TIMER_NUM_E timer_id, TUYA_TIMER_BASE_CFG_T *cfg)
{
    // --- BEGIN: user implements ---
    if (timer_id >= MAX_TUYA_TIMERS || cfg == NULL) {
        return OPRT_INVALID_PARM;
    }

    // [DYNAMIC LOOKUP]
    // Since we didn't add "Timer" to tkl_board_config.h earlier, 
    // we assume a 1:1 mapping if the user didn't request a specific mapping feature for Timers.
    // However, to be consistent, we SHOULD have added it. 
    // For now, I will use a direct cast assuming the Tuya ID matches the TI Index, 
    // OR we can add `tkl_hw_get_timer_index` to board_config later.
    //
    // Strategy: Use direct ID for now as Timers are often virtual or scarce.
    int16_t ti_index = (int16_t)timer_id; 

    // If already open, close it to re-configure
    if (g_timer_ctx[timer_id].handle != NULL) {
        Timer_close(g_timer_ctx[timer_id].handle);
        g_timer_ctx[timer_id].handle = NULL;
    }

    Timer_Params params;
    Timer_Params_init(&params);

    if (cfg->mode == TUYA_TIMER_MODE_ONCE) {
        params.timerMode = Timer_ONESHOT_CALLBACK;
    } else {
        params.timerMode = Timer_CONTINUOUS_CALLBACK;
    }

    params.periodUnits = Timer_PERIOD_US;
    params.period = 1000000; 
    params.timerCallback = _ti_timer_callback_fxn;

    g_timer_ctx[timer_id].cb = cfg->cb;
    g_timer_ctx[timer_id].args = cfg->args;

    // Open TI Driver
    g_timer_ctx[timer_id].handle = Timer_open(ti_index, &params);

    if (g_timer_ctx[timer_id].handle == NULL) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_timer_start(TUYA_TIMER_NUM_E timer_id, uint32_t us)
{
    // --- BEGIN: user implements ---
    if (timer_id >= MAX_TUYA_TIMERS || g_timer_ctx[timer_id].handle == NULL) {
        return OPRT_INVALID_PARM;
    }

    if (Timer_setPeriod(g_timer_ctx[timer_id].handle, Timer_PERIOD_US, us) != Timer_STATUS_SUCCESS) {
        return OPRT_COM_ERROR;
    }
    
    g_timer_ctx[timer_id].current_period_us = us;

    if (Timer_start(g_timer_ctx[timer_id].handle) == Timer_STATUS_SUCCESS) {
        return OPRT_OK;
    }

    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

OPERATE_RET tkl_timer_stop(TUYA_TIMER_NUM_E timer_id)
{
    // --- BEGIN: user implements ---
    if (timer_id >= MAX_TUYA_TIMERS || g_timer_ctx[timer_id].handle == NULL) {
        return OPRT_INVALID_PARM;
    }

    Timer_stop(g_timer_ctx[timer_id].handle);
    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_timer_deinit(TUYA_TIMER_NUM_E timer_id)
{
    // --- BEGIN: user implements ---
    if (timer_id >= MAX_TUYA_TIMERS) {
        return OPRT_INVALID_PARM;
    }

    if (g_timer_ctx[timer_id].handle != NULL) {
        Timer_close(g_timer_ctx[timer_id].handle);
        g_timer_ctx[timer_id].handle = NULL;
    }
    
    g_timer_ctx[timer_id].cb = NULL;
    g_timer_ctx[timer_id].args = NULL;
    g_timer_ctx[timer_id].current_period_us = 0;

    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_timer_get_current_value(TUYA_TIMER_NUM_E timer_id, uint32_t *us)
{
    // --- BEGIN: user implements ---
    if (timer_id >= MAX_TUYA_TIMERS || g_timer_ctx[timer_id].handle == NULL || us == NULL) {
        return OPRT_INVALID_PARM;
    }

    uint32_t counts = Timer_getCount(g_timer_ctx[timer_id].handle);
    *us = counts; 

    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_timer_get(TUYA_TIMER_NUM_E timer_id, uint32_t *us)
{
    // --- BEGIN: user implements ---
    if (timer_id >= MAX_TUYA_TIMERS || us == NULL) {
        return OPRT_INVALID_PARM;
    }
    
    *us = g_timer_ctx[timer_id].current_period_us;
    return OPRT_OK;
    // --- END: user implements ---
}