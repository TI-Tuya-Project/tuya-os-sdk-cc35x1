/**
 * @file tkl_spi.c
 * @brief Tuya Kernel Layer - SPI Implementation
 */

// --- BEGIN: user defines and implements ---
#include "tkl_spi.h"
#include "tuya_error_code.h"
#include <ti/drivers/SPI.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// Dependency Injection
#include "tkl_board_config.h"

// Forward declaration
static void spi_internal_callback(SPI_Handle handle, SPI_Transaction *transaction);

typedef struct {
    SPI_Handle handle;
    SPI_Params params;
    TUYA_SPI_BASE_CFG_T last_cfg;

    TUYA_SPI_IRQ_CB irq_cb;
    bool irq_enabled;

    volatile bool busy;
    volatile uint32_t last_count_frames;
} tkl_spi_ctx_t;

// Use the limit from board config
static tkl_spi_ctx_t g_spi[TKL_HW_MAX_SPI_PORTS] = {0};

static inline bool is_valid_port(TUYA_SPI_NUM_E port) {
    return (port < TKL_HW_MAX_SPI_PORTS);
}

static inline size_t frame_bytes(const tkl_spi_ctx_t *ctx) {
    return (ctx->params.dataSize <= 8) ? 1u : 2u;
}
// --- END: user defines and implements ---

OPERATE_RET tkl_spi_init(TUYA_SPI_NUM_E port, const TUYA_SPI_BASE_CFG_T *cfg)
{
    if (!is_valid_port(port) || cfg == NULL) {
        return OPRT_INVALID_PARM;
    }

    // [DYNAMIC LOOKUP]
    int16_t ti_index = tkl_hw_get_spi_index(port);
    if (ti_index < 0) {
        return OPRT_NOT_SUPPORTED;
    }

    tkl_spi_ctx_t *ctx = &g_spi[port];

    if (ctx->handle != NULL) {
        SPI_close(ctx->handle);
        ctx->handle = NULL;
    }

    SPI_init();
    SPI_Params_init(&ctx->params);

    /* map data bits */
    if (cfg->databits == TUYA_SPI_DATA_BIT8) {
        ctx->params.dataSize = 8;
    } else if (cfg->databits == TUYA_SPI_DATA_BIT16) {
        ctx->params.dataSize = 16;
    } else {
        return OPRT_NOT_SUPPORTED;
    }

    /* map bitrate */
    ctx->params.bitRate = (cfg->freq_hz > 0) ? cfg->freq_hz : 1000000;

    /* map SPI mode */
    switch (cfg->mode) {
        case TUYA_SPI_MODE0: ctx->params.frameFormat = SPI_POL0_PHA0; break;
        case TUYA_SPI_MODE1: ctx->params.frameFormat = SPI_POL0_PHA1; break;
        case TUYA_SPI_MODE2: ctx->params.frameFormat = SPI_POL1_PHA0; break;
        case TUYA_SPI_MODE3: ctx->params.frameFormat = SPI_POL1_PHA1; break;
        default: return OPRT_INVALID_PARM;
    }

    /* map role */
    if (cfg->role == TUYA_SPI_ROLE_SLAVE || cfg->role == TUYA_SPI_ROLE_SLAVE_SIMPLEX) {
        ctx->params.mode = SPI_PERIPHERAL;
    } else {
        ctx->params.mode = SPI_CONTROLLER;
    }

    ctx->params.transferMode = SPI_MODE_BLOCKING;
    ctx->params.transferTimeout = SPI_WAIT_FOREVER;
    ctx->params.transferCallbackFxn = NULL;

    ctx->handle = SPI_open((uint_least8_t)ti_index, &ctx->params);
    if (ctx->handle == NULL) {
        return OPRT_COM_ERROR;
    }

    ctx->last_cfg = *cfg;
    ctx->busy = false;
    ctx->last_count_frames = 0;
    ctx->irq_cb = NULL;
    ctx->irq_enabled = false;

    return OPRT_OK;
}

OPERATE_RET tkl_spi_deinit(TUYA_SPI_NUM_E port)
{
    if (!is_valid_port(port)) return OPRT_INVALID_PARM;

    tkl_spi_ctx_t *ctx = &g_spi[port];

    if (ctx->handle == NULL) return OPRT_OK;

    SPI_transferCancel(ctx->handle);
    SPI_close(ctx->handle);
    ctx->handle = NULL;
    ctx->irq_enabled = false;
    ctx->irq_cb = NULL;

    return OPRT_OK;
}

OPERATE_RET tkl_spi_send(TUYA_SPI_NUM_E port, void *data, uint32_t size)
{
    if (!is_valid_port(port) || data == NULL || size == 0) return OPRT_INVALID_PARM;

    tkl_spi_ctx_t *ctx = &g_spi[port];
    if (ctx->handle == NULL) return OPRT_NOT_SUPPORTED;

    SPI_Transaction txn;
    memset(&txn, 0, sizeof(txn));
    txn.count = size / frame_bytes(ctx);
    txn.txBuf = data;
    txn.rxBuf = NULL;

    ctx->busy = true;
    bool ok = SPI_transfer(ctx->handle, &txn);
    ctx->busy = false;
    ctx->last_count_frames = txn.count;

    return ok ? OPRT_OK : OPRT_COM_ERROR;
}

OPERATE_RET tkl_spi_recv(TUYA_SPI_NUM_E port, void *data, uint32_t size)
{
    if (!is_valid_port(port) || data == NULL || size == 0) return OPRT_INVALID_PARM;

    tkl_spi_ctx_t *ctx = &g_spi[port];
    if (ctx->handle == NULL) return OPRT_NOT_SUPPORTED;

    SPI_Transaction txn;
    memset(&txn, 0, sizeof(txn));
    txn.count = size / frame_bytes(ctx);
    txn.txBuf = NULL;
    txn.rxBuf = data;

    ctx->busy = true;
    bool ok = SPI_transfer(ctx->handle, &txn);
    ctx->busy = false;
    ctx->last_count_frames = txn.count;

    return ok ? OPRT_OK : OPRT_COM_ERROR;
}

OPERATE_RET tkl_spi_transfer(TUYA_SPI_NUM_E port, void *send_buf, void *receive_buf, uint32_t length)
{
    if (!is_valid_port(port) || length == 0) return OPRT_INVALID_PARM;

    tkl_spi_ctx_t *ctx = &g_spi[port];
    if (ctx->handle == NULL) return OPRT_NOT_SUPPORTED;

    SPI_Transaction txn;
    memset(&txn, 0, sizeof(txn));
    txn.count = length / frame_bytes(ctx);
    txn.txBuf = send_buf;
    txn.rxBuf = receive_buf;

    ctx->busy = true;
    bool ok = SPI_transfer(ctx->handle, &txn);
    ctx->busy = false;
    ctx->last_count_frames = txn.count;

    return ok ? OPRT_OK : OPRT_COM_ERROR;
}

// ... Keep other SPI functions (transfer_with_length, abort, irq, etc) similar to before ...
// For brevity, just ensuring Init/Deinit/Send/Recv/Transfer use the mapping.
// The internal callback logic remains valid.

static void spi_internal_callback(SPI_Handle handle, SPI_Transaction *transaction)
{
    for (int port = 0; port < TKL_HW_MAX_SPI_PORTS; ++port) {
        tkl_spi_ctx_t *ctx = &g_spi[port];
        if (ctx->handle == handle) {
            ctx->last_count_frames = transaction ? (uint32_t)transaction->count : 0;
            ctx->busy = false;
            if (ctx->irq_enabled && ctx->irq_cb) {
                ctx->irq_cb((TUYA_SPI_NUM_E)port);
            }
            break;
        }
    }
}