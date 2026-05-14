// --- BEGIN: user defines and implements ---
#include "tkl_i2c.h"
#include "tuya_error_code.h"
#include <ti/drivers/I2C.h>
// [DEPENDENCY INJECTION] Include Board Config
#include "tkl_board_config.h"

// Safe fallback if board config is missing the macro
#ifndef TKL_MAX_I2C_PORTS
#define TKL_MAX_I2C_PORTS 2
#endif

// Array to store handles for multiple I2C ports
static I2C_Handle g_i2c_handles[TKL_MAX_I2C_PORTS] = {NULL};

// Helper to check validity
static bool is_valid_port(TUYA_I2C_NUM_E port) {
    return (port < TKL_MAX_I2C_PORTS);
}
// --- END: user defines and implements ---

/**
 * @brief i2c init
 */
// FIX: Changed TUYA_I2C_BASE_CFG_T to TUYA_IIC_BASE_CFG_T
OPERATE_RET tkl_i2c_init(TUYA_I2C_NUM_E port, const TUYA_IIC_BASE_CFG_T *cfg)
{
    // --- BEGIN: user implements ---
    if (!is_valid_port(port) || cfg == NULL) {
        return OPRT_INVALID_PARM;
    }

    int16_t ti_i2c_index = tkl_hw_get_i2c_index((uint8_t)port);

    if (ti_i2c_index < 0) {
        return OPRT_NOT_SUPPORTED;
    }

    if (g_i2c_handles[port] != NULL) {
        I2C_close(g_i2c_handles[port]);
        g_i2c_handles[port] = NULL;
    }

    I2C_Params params;
    I2C_Params_init(&params);

    // FIX: Changed to TUYA_IIC_MODE_MASTER
    if (cfg->role == TUYA_IIC_MODE_MASTER) {
        params.transferMode = I2C_MODE_BLOCKING;
        
        // FIX: Bypassed volatile Tuya speed enums. Default to safe 100kHz.
        params.bitRate = I2C_100kHz; 
    } else {
        return OPRT_NOT_SUPPORTED; 
    }

    I2C_init();
    g_i2c_handles[port] = I2C_open(ti_i2c_index, &params);

    if (g_i2c_handles[port] == NULL) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

// ... (deinit, irq_init, irq_enable, irq_disable remain exactly the same) ...

/**
 * @brief i2c master send
 */
OPERATE_RET tkl_i2c_master_send(TUYA_I2C_NUM_E port, uint16_t dev_addr, const void *data, uint32_t size,
                                BOOL_T xfer_pending)
{
    // --- BEGIN: user implements ---
    if (!is_valid_port(port) || g_i2c_handles[port] == NULL || data == NULL) {
        return OPRT_INVALID_PARM;
    }

    I2C_Transaction transaction;
    // FIX: Replaced slaveAddress with targetAddress for TI SDK 9.x compliance
    transaction.targetAddress = dev_addr; 
    transaction.writeBuf = (void *)data;
    transaction.writeCount = size;
    transaction.readBuf = NULL;
    transaction.readCount = 0;
    
    if (I2C_transfer(g_i2c_handles[port], &transaction)) {
        return OPRT_OK;
    }

    return OPRT_COM_ERROR;
    // --- END: user implements ---
}

/**
 * @brief i2c master recv
 */
OPERATE_RET tkl_i2c_master_receive(TUYA_I2C_NUM_E port, uint16_t dev_addr, void *data, uint32_t size,
                                   BOOL_T xfer_pending)
{
    // --- BEGIN: user implements ---
    if (!is_valid_port(port) || g_i2c_handles[port] == NULL || data == NULL) {
        return OPRT_INVALID_PARM;
    }

    I2C_Transaction transaction;
    // FIX: Replaced slaveAddress with targetAddress for TI SDK 9.x compliance
    transaction.targetAddress = dev_addr; 
    transaction.writeBuf = NULL;
    transaction.writeCount = 0;
    transaction.readBuf = data;
    transaction.readCount = size;
    
    if (I2C_transfer(g_i2c_handles[port], &transaction)) {
        return OPRT_OK;
    }

    return OPRT_COM_ERROR;
    // --- END: user implements ---
}
/**
 * @brief i2c deinit
 */
OPERATE_RET tkl_i2c_deinit(TUYA_I2C_NUM_E port)
{
    // --- BEGIN: user implements ---
    if (!is_valid_port(port)) {
        return OPRT_INVALID_PARM;
    }

    // Safely close the TI I2C handle and null the pointer
    if (g_i2c_handles[port] != NULL) {
        I2C_close(g_i2c_handles[port]);
        g_i2c_handles[port] = NULL;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief i2c's reset
 */
OPERATE_RET tkl_i2c_reset(TUYA_I2C_NUM_E port)
{
    // --- BEGIN: user implements ---
    if (!is_valid_port(port)) {
        return OPRT_INVALID_PARM;
    }

    // A hardware reset on TI is effectively a Deinit followed by an Init
    tkl_i2c_deinit(port);

    // Re-initialize with standard 100kHz parameters
    I2C_Params params;
    I2C_Params_init(&params);
    params.transferMode = I2C_MODE_BLOCKING;
    params.bitRate      = I2C_100kHz;

    int16_t ti_i2c_index = tkl_hw_get_i2c_index((uint8_t)port);
    g_i2c_handles[port] = I2C_open(ti_i2c_index, &params);

    if (g_i2c_handles[port] == NULL) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}