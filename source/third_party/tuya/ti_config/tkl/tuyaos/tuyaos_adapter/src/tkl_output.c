/**
 * @file tkl_output.c
 * @brief Auto-generated adapter skeleton with user implementations between BEGIN and END.
 *
 * @warning: changes between user 'BEGIN' and 'END' will be keeped when run tuyaos v&v tools
 * changes in other place will be overwrited and lost
 *
 * @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
 *
 */

/* Adapter-specific includes and definitions. */
#include "tkl_output.h"
#include "tuya_error_code.h"

#include <ti/drivers/UART2.h>
#include <string.h>

/* Board mapping used to resolve the log UART instance. */
#include "tkl_board_config.h"
#include <stdio.h>
#include <stdarg.h>

extern void tuya_cli_print_bridge(const char *str);

/* Default Tuya log port. */
#define TUYA_LOG_PORT_ID 0

static UART2_Handle uartHandle = NULL;

/**
 * @brief Output log information
 *
 * @pre     tkl_log_open has been called
 * @pre     There are no ongoing read or write calls.  Any ongoing read
 * or write calls can be cancelled with UART2_readCancel() or
 * UART2_writeCancel().
 *
 * @param[in] str: log information
 *
 * @note This API is used for outputing log information
 *
 * @return
 */
void tkl_log_output(const char *str, ...)
{
    if (str == NULL || uartHandle == NULL) {
        return;
    }

    char log_buf[256];
    va_list args;

    va_start(args, str);
    vsnprintf(log_buf, sizeof(log_buf), str, args);
    va_end(args);

    size_t bytesWritten = 0;
    UART2_write(uartHandle, log_buf, strlen(log_buf), &bytesWritten);
}

/**
 * @brief Close log port
 *
 * @pre    tkl_log_open has been called.
 * @pre    There are no ongoing read or write calls.  Any ongoing read
 * or write calls can be cancelled with UART2_readCancel() or
 * UART2_writeCancel().
 *
 * @param void
 *
 * @note This API is used for closing log port.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_log_close(void)
{
    /* Cancel any pending UART activity before closing. */
    UART2_readCancel(uartHandle);
    UART2_writeCancel(uartHandle);

    if (uartHandle != NULL) {
        UART2_close(uartHandle);
        uartHandle = NULL;
    }

    return OPRT_OK;
}

/**
 * @brief Open log port
 *
 * @param void
 *
 * @note This API is used for openning log port.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_log_open(void)
{
    /* Look up the TI UART index mapped to the log port. */
    int16_t ti_log_index = tkl_hw_get_uart_index(TUYA_LOG_PORT_ID);

    if (ti_log_index < 0) {
        return OPRT_NOT_SUPPORTED;
    }

    UART2_Params params;
    UART2_Params_init(&params);

    params.baudRate = 115200;
    params.writeMode = UART2_Mode_BLOCKING;

    uartHandle = UART2_open(ti_log_index, &params);

    if (uartHandle == NULL) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
}
