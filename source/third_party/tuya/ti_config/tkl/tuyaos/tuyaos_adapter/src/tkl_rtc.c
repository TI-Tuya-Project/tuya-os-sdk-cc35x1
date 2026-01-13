/**
 * @file tkl_rtc.c
 * @brief Tuya Kernel Layer for RTC on TI SimpleLink (using Seconds module)
 */

// --- BEGIN: user defines and implements ---
#include "tkl_rtc.h"
#include "tuya_error_code.h"

/* TI Drivers includes for RTC/Seconds management */
#include <ti/drivers/dpl/Seconds.h>
#include <time.h>
// --- END: user defines and implements ---

/**
 * @brief rtc init
 *
 * @param[in] none
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_rtc_init(void)
{
    // --- BEGIN: user implements ---
    /* * TI's Seconds module is typically initialized by the OS or Board init.
     * We don't need explicit hardware initialization here for SimpleLink.
     * Just returning OK signals to Tuya that RTC is ready.
     */
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief rtc deinit
 * @param[in] none
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_rtc_deinit(void)
{
    // --- BEGIN: user implements ---
    /* * RTC usually runs continuously. There is no standard "stop" 
     * for the Seconds module in TI DPL, nor is it usually required.
     */
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief rtc time set
 *
 * @param[in] time_sec: rtc time seconds
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_rtc_time_set(TIME_T time_sec)
{
    // --- BEGIN: user implements ---
    /* Set the seconds on the TI RTC hardware */
    Seconds_set((uint32_t)time_sec);
    
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief rtc time get
 *
 * @param[in] time_sec:rtc time seconds
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_rtc_time_get(TIME_T *time_sec)
{
    // --- BEGIN: user implements ---
    if (time_sec == NULL) {
        return OPRT_INVALID_PARM;
    }

    /* Get the seconds from the TI RTC hardware */
    *time_sec = (TIME_T)Seconds_get();
    
    return OPRT_OK;
    // --- END: user implements ---
}