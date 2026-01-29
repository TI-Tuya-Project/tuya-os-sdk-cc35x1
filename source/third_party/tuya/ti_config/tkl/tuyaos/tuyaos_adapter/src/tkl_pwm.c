/**
 * @file tkl_pwm.c
 * @brief Tuya Kernel Layer - PWM Implementation
 */

// --- BEGIN: user defines and implements ---
#include "tkl_pwm.h"
#include "tuya_error_code.h"
#include <ti/drivers/PWM.h> 

// Dependency Injection
#include "tkl_board_config.h"

// Helper to check validity using the Board Config Limit
#define CHECK_PWM_ID(ch) \
    do { \
        if ((ch) >= TKL_HW_MAX_PWM_CHANNELS) return OPRT_INVALID_PARM; \
    } while(0)

static PWM_Handle g_pwm_handles[TKL_HW_MAX_PWM_CHANNELS] = {NULL};

#define TUYA_PWM_MAX_DUTY 10000
// --- END: user defines and implements ---

OPERATE_RET tkl_pwm_init(TUYA_PWM_NUM_E ch_id, const TUYA_PWM_BASE_CFG_T *cfg)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_ID(ch_id);
    if (cfg == NULL) return OPRT_INVALID_PARM;

    // 1. Get Mapped Index
    int16_t ti_index = tkl_hw_get_pwm_index(ch_id);
    if (ti_index < 0) return OPRT_NOT_SUPPORTED;

    // 2. Open Driver if not already open
    if (g_pwm_handles[ch_id] == NULL) {
        PWM_Params params;
        PWM_Params_init(&params);
        
        params.periodUnits = PWM_PERIOD_HZ;
        params.periodValue = cfg->frequency;
        params.dutyUnits = PWM_DUTY_FRACTION;
        
        // Calculate Duty
        uint64_t duty_calc = (uint64_t)cfg->duty * (uint64_t)PWM_DUTY_FRACTION_MAX;
        params.dutyValue = (uint32_t)(duty_calc / TUYA_PWM_MAX_DUTY);
        
        params.idleLevel = (cfg->polarity == TUYA_PWM_POLARITY_ACTIVE_HIGH) ? PWM_IDLE_LOW : PWM_IDLE_HIGH;

        g_pwm_handles[ch_id] = PWM_open(ti_index, &params);
    }
    
    if (g_pwm_handles[ch_id] == NULL) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_pwm_deinit(TUYA_PWM_NUM_E ch_id)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_ID(ch_id);
    if (g_pwm_handles[ch_id]) {
        PWM_close(g_pwm_handles[ch_id]);
        g_pwm_handles[ch_id] = NULL;
    }
    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_pwm_start(TUYA_PWM_NUM_E ch_id)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_ID(ch_id);
    if (g_pwm_handles[ch_id] == NULL) return OPRT_COM_ERROR;
    PWM_start(g_pwm_handles[ch_id]);
    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_pwm_stop(TUYA_PWM_NUM_E ch_id)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_ID(ch_id);
    if (g_pwm_handles[ch_id] == NULL) return OPRT_COM_ERROR;
    PWM_stop(g_pwm_handles[ch_id]);
    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_pwm_multichannel_start(TUYA_PWM_NUM_E *ch_id, uint8_t num)
{
    // --- BEGIN: user implements ---
    if (!ch_id) return OPRT_INVALID_PARM;
    for(int i = 0; i < num; ++i) tkl_pwm_start(ch_id[i]);
    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_pwm_multichannel_stop(TUYA_PWM_NUM_E *ch_id, uint8_t num)
{
    // --- BEGIN: user implements ---
    if (!ch_id) return OPRT_INVALID_PARM;
    for(int i = 0; i < num; ++i) tkl_pwm_stop(ch_id[i]);
    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_pwm_duty_set(TUYA_PWM_NUM_E ch_id, uint32_t duty)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_ID(ch_id);
    if (g_pwm_handles[ch_id] == NULL) return OPRT_COM_ERROR;

    uint64_t duty_calc = (uint64_t)duty * (uint64_t)PWM_DUTY_FRACTION_MAX;
    uint32_t ti_duty = (uint32_t)(duty_calc / TUYA_PWM_MAX_DUTY);

    if (PWM_setDuty(g_pwm_handles[ch_id], ti_duty) < 0) return OPRT_COM_ERROR;
    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_pwm_frequency_set(TUYA_PWM_NUM_E ch_id, uint32_t frequency)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_ID(ch_id);
    if (g_pwm_handles[ch_id] == NULL) return OPRT_COM_ERROR;
    if (PWM_setPeriod(g_pwm_handles[ch_id], frequency) < 0) return OPRT_COM_ERROR;
    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_pwm_polarity_set(TUYA_PWM_NUM_E ch_id, TUYA_PWM_POLARITY_E polarity)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}

OPERATE_RET tkl_pwm_info_set(TUYA_PWM_NUM_E ch_id, const TUYA_PWM_BASE_CFG_T *info)
{
    // --- BEGIN: user implements ---
    tkl_pwm_stop(ch_id);
    tkl_pwm_deinit(ch_id);
    return tkl_pwm_init(ch_id, info);
    // --- END: user implements ---
}

OPERATE_RET tkl_pwm_info_get(TUYA_PWM_NUM_E ch_id, TUYA_PWM_BASE_CFG_T *info)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}

OPERATE_RET tkl_pwm_cap_start(TUYA_PWM_NUM_E ch_id, const TUYA_PWM_CAP_IRQ_T *cfg)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}

OPERATE_RET tkl_pwm_cap_stop(TUYA_PWM_NUM_E ch_id)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}