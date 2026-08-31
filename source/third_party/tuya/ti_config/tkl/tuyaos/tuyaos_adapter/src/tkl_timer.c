/* Adapter-specific includes and definitions. */
#include "tkl_timer.h"
#include "tuya_error_code.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "timers.h"
#include <stddef.h>

#define MAX_TUYA_TIMERS 4

typedef struct {
    TimerHandle_t handle;
    TUYA_TIMER_ISR_CB cb;
    void *args;
    uint32_t period_us;
} tuya_timer_ctx_t;

static tuya_timer_ctx_t g_timer_ctx[MAX_TUYA_TIMERS] = {0};

/* FreeRTOS Timer Callback Wrapper */
static void _freertos_timer_cb(TimerHandle_t xTimer)
{
    // Retrieve the timer ID we stored during creation
    uint32_t timer_id = (uint32_t)pvTimerGetTimerID(xTimer);

    if (timer_id < MAX_TUYA_TIMERS && g_timer_ctx[timer_id].cb != NULL) {
        g_timer_ctx[timer_id].cb(g_timer_ctx[timer_id].args);
    }
}



OPERATE_RET tkl_timer_init(TUYA_TIMER_NUM_E timer_id, TUYA_TIMER_BASE_CFG_T *cfg)
{
    if (timer_id >= MAX_TUYA_TIMERS || cfg == NULL) {
        return OPRT_INVALID_PARM;
    }

    if (g_timer_ctx[timer_id].handle != NULL) {
        xTimerDelete(g_timer_ctx[timer_id].handle, 0);
        g_timer_ctx[timer_id].handle = NULL;
    }

    // pdTRUE for continuous, pdFALSE for one-shot
    UBaseType_t auto_reload = (cfg->mode == TUYA_TIMER_MODE_PERIOD) ? pdTRUE : pdFALSE;

    // Create the FreeRTOS Software Timer (Default to 1 tick, will change on start)
    g_timer_ctx[timer_id].handle = xTimerCreate("tuya_tmr", 1, auto_reload, (void *)timer_id, _freertos_timer_cb);

    if (g_timer_ctx[timer_id].handle == NULL) {
        return OPRT_COM_ERROR;
    }

    g_timer_ctx[timer_id].cb = cfg->cb;
    g_timer_ctx[timer_id].args = cfg->args;

    return OPRT_OK;
}

OPERATE_RET tkl_timer_start(TUYA_TIMER_NUM_E timer_id, uint32_t us)
{
    if (timer_id >= MAX_TUYA_TIMERS || g_timer_ctx[timer_id].handle == NULL) {
        return OPRT_INVALID_PARM;
    }

    // Convert microseconds to FreeRTOS OS Ticks (ms = us / 1000)
    uint32_t ms = us / 1000;
    if (ms == 0) ms = 1; // Minimum 1ms

    TickType_t ticks = ms / portTICK_PERIOD_MS;
    if (ticks == 0) ticks = 1;

    g_timer_ctx[timer_id].period_us = us;

    // Update the period and start the timer
    if (xTimerChangePeriod(g_timer_ctx[timer_id].handle, ticks, 0) != pdPASS) {
        return OPRT_COM_ERROR;
    }

    if (xTimerStart(g_timer_ctx[timer_id].handle, 0) != pdPASS) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
}

OPERATE_RET tkl_timer_stop(TUYA_TIMER_NUM_E timer_id)
{
    if (timer_id >= MAX_TUYA_TIMERS || g_timer_ctx[timer_id].handle == NULL) {
        return OPRT_INVALID_PARM;
    }

    xTimerStop(g_timer_ctx[timer_id].handle, 0);
    return OPRT_OK;
}

OPERATE_RET tkl_timer_deinit(TUYA_TIMER_NUM_E timer_id)
{
    if (timer_id >= MAX_TUYA_TIMERS) {
        return OPRT_INVALID_PARM;
    }

    if (g_timer_ctx[timer_id].handle != NULL) {
        xTimerDelete(g_timer_ctx[timer_id].handle, 0);
        g_timer_ctx[timer_id].handle = NULL;
    }

    g_timer_ctx[timer_id].cb = NULL;
    g_timer_ctx[timer_id].args = NULL;

    return OPRT_OK;
}

OPERATE_RET tkl_timer_get(TUYA_TIMER_NUM_E timer_id, uint32_t *us)
{
    if (timer_id >= MAX_TUYA_TIMERS || us == NULL) {
        return OPRT_INVALID_PARM;
    }

    *us = g_timer_ctx[timer_id].period_us;
    return OPRT_OK;
}

OPERATE_RET tkl_timer_get_current_value(TUYA_TIMER_NUM_E timer_id, uint32_t *us)
{
    if (timer_id >= MAX_TUYA_TIMERS || us == NULL) {
        return OPRT_INVALID_PARM;
    }

    *us = 0; // Stub: FreeRTOS doesn't easily expose elapsed tick time for a running timer
    return OPRT_OK;
}
