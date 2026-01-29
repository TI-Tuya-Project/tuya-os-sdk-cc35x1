/**
 * @file tkl_output.c
 * @brief Tuya Kernel Layer - Log Output
 */

// --- BEGIN: user defines and implements ---
#include "tkl_output.h"
#include "tuya_error_code.h"
#include <ti/drivers/UART2.h>
#include <string.h>

// Dependency Injection
#include "tkl_board_config.h"

// We assume Tuya Log is always Port 0
#define TUYA_LOG_PORT 0

static UART2_Handle g_log_uart = NULL;
// --- END: user defines and implements ---

void tkl_log_output(const char *str, ...)
{
    // --- BEGIN: user implements ---
    if (str == NULL || g_log_uart == NULL) {
        return;
    }

    size_t bytesWritten = 0;
    // Note: Tuya Core formats string before calling this.
    UART2_write(g_log_uart, str, strlen(str), &bytesWritten);
    // --- END: user implements ---
}

OPERATE_RET tkl_log_close(void)
{
    // --- BEGIN: user implements ---
    if (g_log_uart != NULL) {
        UART2_close(g_log_uart);
        g_log_uart = NULL;
    }
    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_log_open(void)
{
    // --- BEGIN: user implements ---
    if (g_log_uart != NULL) {
        return OPRT_OK; 
    }

    // 1. Get TI Hardware Index from Board Config
    int16_t ti_index = tkl_hw_get_uart_index(TUYA_LOG_PORT);
    
    if (ti_index < 0) {
        // App didn't map a log port!
        return OPRT_COM_ERROR;
    }

    UART2_Params params;
    UART2_Params_init(&params);
    params.baudRate = 115200;
    params.writeMode = UART2_Mode_BLOCKING; // Blocking is safer for logs

    g_log_uart = UART2_open(ti_index, &params);

    if (g_log_uart == NULL) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}