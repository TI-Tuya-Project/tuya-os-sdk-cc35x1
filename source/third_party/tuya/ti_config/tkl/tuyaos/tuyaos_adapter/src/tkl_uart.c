/**
 * @file tkl_uart.c
 * @brief Tuya Kernel Layer - UART Implementation
 */

// --- BEGIN: user defines and implements ---
#include "tkl_uart.h"
#include "tuya_error_code.h"
#include <ti/drivers/UART2.h>
#include <stddef.h>

// Dependency Injection
#include "tkl_board_config.h"

// Array to store handles. Uses the max defined in board config.
static UART2_Handle g_uart_handles[TKL_HW_MAX_UART_PORTS] = {NULL};

#define CHECK_UART_ID(port) \
    if ((port) >= TKL_HW_MAX_UART_PORTS) return OPRT_INVALID_PARM;

// --- END: user defines and implements ---

OPERATE_RET tkl_uart_init(TUYA_UART_NUM_E port_id, TUYA_UART_BASE_CFG_T *cfg)
{
    // --- BEGIN: user implements ---
    CHECK_UART_ID(port_id);
    if (cfg == NULL) return OPRT_INVALID_PARM;

    // 1. Get Mapped Index
    int16_t ti_index = tkl_hw_get_uart_index(port_id);
    if (ti_index < 0) return OPRT_NOT_SUPPORTED;

    // 2. Prepare Config
    UART2_Params uartParams;
    UART2_Params_init(&uartParams);

    uartParams.baudRate = cfg->baudrate;
    
    if (cfg->parity == TUYA_UART_PARITY_TYPE_NONE) {
        uartParams.parityType = UART2_Parity_NONE;
    } else if (cfg->parity == TUYA_UART_PARITY_TYPE_EVEN) {
        uartParams.parityType = UART2_Parity_EVEN;
    } else if (cfg->parity == TUYA_UART_PARITY_TYPE_ODD) {
        uartParams.parityType = UART2_Parity_ODD;
    }

    if (cfg->stopbits == TUYA_UART_STOP_LEN_1BIT) {
        uartParams.stopBits = UART2_StopBits_1;
    } else if (cfg->stopbits == TUYA_UART_STOP_LEN_2BIT) {
        uartParams.stopBits = UART2_StopBits_2;
    }

    if (cfg->databits == TUYA_UART_DATA_LEN_8BIT) {
        uartParams.dataLength = UART2_DataLen_8;
    } else if (cfg->databits == TUYA_UART_DATA_LEN_7BIT) {
        uartParams.dataLength = UART2_DataLen_7;
    }

    // Mode: Blocking is used for stability
    uartParams.readMode = UART2_Mode_BLOCKING;
    uartParams.writeMode = UART2_Mode_BLOCKING;
    
    // 3. Close if already open
    if (g_uart_handles[port_id] != NULL) {
        UART2_close(g_uart_handles[port_id]);
        g_uart_handles[port_id] = NULL;
    }

    // 4. Open with Mapped Index
    g_uart_handles[port_id] = UART2_open(ti_index, &uartParams);

    if (g_uart_handles[port_id] == NULL) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_uart_deinit(TUYA_UART_NUM_E port_id)
{
    // --- BEGIN: user implements ---
    CHECK_UART_ID(port_id);
    if (g_uart_handles[port_id] != NULL) {
        UART2_close(g_uart_handles[port_id]);
        g_uart_handles[port_id] = NULL;
    }
    return OPRT_OK;
    // --- END: user implements ---
}

int tkl_uart_write(TUYA_UART_NUM_E port_id, void *buff, uint16_t len)
{
    // --- BEGIN: user implements ---
    if (port_id >= TKL_HW_MAX_UART_PORTS) return -1;
    if (g_uart_handles[port_id] == NULL) return -1;
    
    size_t bytesWritten = 0;
    UART2_write(g_uart_handles[port_id], buff, (size_t)len, &bytesWritten);
    
    return (int)bytesWritten;
    // --- END: user implements ---
}

// ... IRQ functions remain empty as discussed ...

int tkl_uart_read(TUYA_UART_NUM_E port_id, void *buff, uint16_t len)
{
    // --- BEGIN: user implements ---
    if (port_id >= TKL_HW_MAX_UART_PORTS) return -1;
    if (g_uart_handles[port_id] == NULL) return -1;

    size_t bytesRead = 0;
    UART2_read(g_uart_handles[port_id], buff, (size_t)len, &bytesRead);
    
    return (int)bytesRead;
    // --- END: user implements ---
}

// ... Other Flow Control functions remain NOT_SUPPORTED ...
OPERATE_RET tkl_uart_set_tx_int(TUYA_UART_NUM_E port_id, BOOL_T enable)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_uart_set_rx_flowctrl(TUYA_UART_NUM_E port_id, BOOL_T enable)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_uart_wait_for_data(TUYA_UART_NUM_E port_id, int timeout_ms)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_uart_ioctl(TUYA_UART_NUM_E port_id, uint32_t cmd, void *arg)
{
    return OPRT_NOT_SUPPORTED;
}