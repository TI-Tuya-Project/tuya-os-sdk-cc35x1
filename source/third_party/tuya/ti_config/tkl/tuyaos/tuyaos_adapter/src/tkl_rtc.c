/**
 * @file tkl_rtc.c
 * @brief Tuya Kernel Layer for RTC on TI SimpleLink (FreeRTOS Soft-RTC)
 */

/* Adapter-specific includes and definitions. */
#include "tkl_rtc.h"
#include "tuya_error_code.h"

/* We use FreeRTOS to handle the time tracking */
#include "FreeRTOS.h"
#include "task.h"

static TIME_T g_rtc_base_time = 0;
static uint32_t g_rtc_base_ticks = 0;


/**
 * @brief rtc init
 *
 * @param[in] none
 *
 * @return OPRT_OK on success. Others on error
 */
OPERATE_RET tkl_rtc_init(void)
{
    // --- BEGIN: user implements ---
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief rtc deinit
 * @param[in] none
 * @return OPRT_OK on success. Others on error
 */
OPERATE_RET tkl_rtc_deinit(void)
{
    // --- BEGIN: user implements ---
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief rtc time set
 *
 * @param[in] time_sec: rtc time seconds
 *
 * @return OPRT_OK on success. Others on error
 */
OPERATE_RET tkl_rtc_time_set(TIME_T time_sec)
{
    // --- BEGIN: user implements ---
    // Save the exact network time and the exact CPU tick it was received
    g_rtc_base_time = time_sec;
    g_rtc_base_ticks = xTaskGetTickCount();

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief rtc time get
 *
 * @param[in] time_sec:rtc time seconds
 *
 * @return OPRT_OK on success. Others on error
 */
OPERATE_RET tkl_rtc_time_get(TIME_T *time_sec)
{
    // --- BEGIN: user implements ---
    if (time_sec == NULL) {
        return OPRT_INVALID_PARM;
    }

    // Get current CPU ticks
    uint32_t current_ticks = xTaskGetTickCount();

    // Calculate how many ticks have passed since we set the time.
    // Unsigned math naturally handles the 32-bit tick rollover safely.
    uint32_t elapsed_ticks = current_ticks - g_rtc_base_ticks;

    // Convert elapsed ticks to seconds
    uint32_t elapsed_sec = elapsed_ticks / configTICK_RATE_HZ;

    // Output the calculated time
    *time_sec = g_rtc_base_time + elapsed_sec;

    return OPRT_OK;
    // --- END: user implements ---
}
