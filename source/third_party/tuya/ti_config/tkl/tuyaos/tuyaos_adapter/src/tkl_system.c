/**
 * @file tkl_system.c
 * @brief Auto-generated adapter skeleton with user implementations between BEGIN and END.
 *
 * @warning: changes between user 'BEGIN' and 'END' will be keeped when run tuyaos v&v tools
 *           changes in other place will be overwrited and lost
 *
 * @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
 *
 */

/* Adapter-specific includes and definitions. */
#include "tkl_system.h"
#include "tuya_error_code.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stdlib.h>
#include <ti/devices/DeviceFamily.h>

/* The reset implementation relies on the Cortex-M CMSIS reset path. */
#ifndef NVIC_SystemReset
/* extern void NVIC_SystemReset(void); */
#endif

/**
 * @brief system reset
 *
 * @param none
 *
 * @return none
 */
void tkl_system_reset(void)
{
    /* Disable interrupts before reset. */
    __asm(" cpsid i ");

    /* Request system reset if the platform reset hook is enabled. */
    /* NVIC_SystemReset(); */

    /* Spin until the reset takes effect. */
    while (1) {}
}

/**
 * @brief Get system tick count
 *
 * @param none
 *
 * @return system tick count
 */
SYS_TICK_T tkl_system_get_tick_count(void)
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return 0;
    }
    return (SYS_TICK_T)xTaskGetTickCount();
}

/**
 * @brief Get system millisecond
 *
 * @param none
 *
 * @return system millisecond
 */
SYS_TIME_T tkl_system_get_millisecond(void)
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return 0;
    }
    return (SYS_TIME_T)xTaskGetTickCount();
}

/**
 * @brief Get system random data
 *
 * @param[in] range: random from 0  to range
 *
 * @return random value
 */
int tkl_system_get_random(uint32_t range)
{
    if (range == 0) return 0;
    return (int)(rand() % range);
}

/**
 * @brief Get system reset reason
 *
 * @param[in] describe: point to reset reason describe
 *
 * @return reset reason
 */
TUYA_RESET_REASON_E tkl_system_get_reset_reason(char **describe)
{
    return TUYA_RESET_REASON_POWERON;
}

/**
 * @brief  system sleep
 *
 * @param[in] describe: num ms
 *
 * @return none
 */
void tkl_system_sleep(uint32_t num_ms)
{
    uint32_t ticks = num_ms / portTICK_PERIOD_MS;
    if (ticks == 0 && num_ms > 0) {
        ticks = 1;
    }
    vTaskDelay(ticks);
}

/**
 * @brief system delay
 *
 * @param[in] msTime: time in MS
 *
 * @note This API is used for system sleep.
 *
 * @return void
 */
void tkl_system_delay(uint32_t num_ms)
{
    tkl_system_sleep(num_ms);
}

/**
 * @brief get system cpu info
 *
 * @param[in] cpu_ary: info of cpus
 * @param[in] cpu_cnt: num of cpu
 * @note This API is used for system cpu info get.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET tkl_system_get_cpu_info(TUYA_CPU_INFO_T **cpu_ary, int *cpu_cnt)
{
    return OPRT_NOT_SUPPORTED;
}

