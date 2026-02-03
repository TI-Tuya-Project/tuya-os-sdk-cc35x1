/**
 * @file tkl_ota.c
 * @brief Tuya Kernel Layer (TKL) OTA adapter.
 *
 * This module receives OTA image fragments from Tuya OTA service and writes them
 * into flash via the TKL flash abstraction (tkl_flash_*).
 *
 * Design notes:
 * - This implementation assumes the OTA package is written to a contiguous flash
 *   area starting at a configured base address (or pack->start_addr if provided).
 * - Erase is performed on-demand before writing, in aligned chunks.
 * - Final verification (CRC/MD5) and AB-boot switching are platform-specific and
 *   must be integrated with the board's bootloader/OAD solution if required.
 *
 * @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
 */

// --- BEGIN: user defines and implements ---
#include "tkl_ota.h"
#include "tkl_flash.h"
#include "tuya_error_code.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
// --- END: user defines and implements ---

/* --------------------------------------------------------------------------
 * Configuration
 * -------------------------------------------------------------------------- */

/* Default OTA storage base address (offset) used when pack->start_addr is 0.
 * This address must point to a valid writable flash region large enough to
 * store the full OTA image. */
#ifndef TKL_OTA_DEFAULT_START_ADDR
#define TKL_OTA_DEFAULT_START_ADDR   (0u)
#endif

/* Erase granularity used by this adapter.
 * If your flash sector size is known, set this macro accordingly (e.g. 4096).
 * The erase size must be compatible with your underlying flash driver. */
#ifndef TKL_OTA_ERASE_UNIT
#define TKL_OTA_ERASE_UNIT           (4096u)
#endif

/* Optional: maximum allowed OTA image size (0 means "unknown/not enforced"). */
#ifndef TKL_OTA_MAX_IMAGE_SIZE
#define TKL_OTA_MAX_IMAGE_SIZE       (0u)
#endif

/* --------------------------------------------------------------------------
 * Internal state
 * -------------------------------------------------------------------------- */

typedef struct {
    bool              active;
    TUYA_OTA_TYPE_E    type;
    TUYA_OTA_PATH_E    path;

    uint32_t          total_len;
    uint32_t          base_addr;

    uint32_t          erased_upto;   /* Offset (bytes) erased from base_addr */
} tkl_ota_state_t;

static tkl_ota_state_t g_ota = {0};

/* --------------------------------------------------------------------------
 * Helpers
 * -------------------------------------------------------------------------- */

static uint32_t _align_down(uint32_t v, uint32_t a)
{
    return (a == 0u) ? v : (v / a) * a;
}

static uint32_t _align_up(uint32_t v, uint32_t a)
{
    if (a == 0u) {
        return v;
    }
    uint32_t rem = v % a;
    return (rem == 0u) ? v : (v + (a - rem));
}

static OPERATE_RET _ota_ensure_erased(uint32_t required_end_offset)
{
    /* required_end_offset is relative to base_addr */
    if (TKL_OTA_ERASE_UNIT == 0u) {
        return OPRT_INVALID_PARM;
    }

    uint32_t target = _align_up(required_end_offset, TKL_OTA_ERASE_UNIT);

    while (g_ota.erased_upto < target) {
        uint32_t erase_addr = g_ota.base_addr + g_ota.erased_upto;
        uint32_t erase_size = TKL_OTA_ERASE_UNIT;

        OPERATE_RET r = tkl_flash_erase(erase_addr, erase_size);
        if (r != OPRT_OK) {
            return r;
        }

        g_ota.erased_upto += erase_size;
    }

    return OPRT_OK;
}

/* --------------------------------------------------------------------------
 * TKL OTA APIs
 * -------------------------------------------------------------------------- */

OPERATE_RET tkl_ota_get_ability(uint32_t *image_size, TUYA_OTA_TYPE_E *type)
{
    // --- BEGIN: user implements ---
    if (image_size == NULL || type == NULL) {
        return OPRT_INVALID_PARM;
    }

    /* Default to FULL. If you support DIFF, you can return TUYA_OTA_DIFF here. */
    *type = TUYA_OTA_FULL;

    /* If a platform-specific max size is known, return it; otherwise 0 means
     * "not provided here" and higher layers may decide. */
    *image_size = TKL_OTA_MAX_IMAGE_SIZE;

    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_ota_start_notify(uint32_t image_size, TUYA_OTA_TYPE_E type, TUYA_OTA_PATH_E path)
{
    // --- BEGIN: user implements ---
    memset(&g_ota, 0, sizeof(g_ota));

    g_ota.active    = true;
    g_ota.type      = type;
    g_ota.path      = path;
    g_ota.total_len = image_size;

#if (TKL_OTA_MAX_IMAGE_SIZE != 0u)
    if (image_size > TKL_OTA_MAX_IMAGE_SIZE) {
        g_ota.active = false;
        return OPRT_COM_ERROR;
    }
#endif

    /* Base address will be selected on first data packet:
     * - Prefer pack->start_addr if non-zero
     * - Otherwise use TKL_OTA_DEFAULT_START_ADDR */
    g_ota.base_addr   = 0u;
    g_ota.erased_upto = 0u;

    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_ota_data_process(TUYA_OTA_DATA_T *pack, uint32_t *remain_len)
{
    // --- BEGIN: user implements ---
    if (pack == NULL || remain_len == NULL) {
        return OPRT_INVALID_PARM;
    }

    if (!g_ota.active) {
        return OPRT_COM_ERROR;
    }

    if (pack->data == NULL || pack->len == 0u) {
        return OPRT_INVALID_PARM;
    }

    /* Select base address. */
    if (g_ota.base_addr == 0u) {
        if (pack->start_addr != 0u) {
            g_ota.base_addr = pack->start_addr;
        } else {
            g_ota.base_addr = TKL_OTA_DEFAULT_START_ADDR;
        }

        /* If still 0, it may be valid in your flash abstraction (offset-based).
         * Keep it as-is; the platform mapping must ensure this goes to the OTA partition. */
        g_ota.erased_upto = 0u;
    }

    /* Basic bounds checks using total_len when available. */
    if (pack->total_len != 0u) {
        g_ota.total_len = pack->total_len;
    }

    if (g_ota.total_len != 0u) {
        if (pack->offset > g_ota.total_len) {
            return OPRT_INVALID_PARM;
        }
        if (pack->len > (g_ota.total_len - pack->offset)) {
            return OPRT_INVALID_PARM;
        }
    }

    /* Ensure target area is erased before write. */
    uint32_t end_offset = pack->offset + pack->len;
    OPERATE_RET r = _ota_ensure_erased(end_offset);
    if (r != OPRT_OK) {
        return r;
    }

    /* Program flash. */
    uint32_t write_addr = g_ota.base_addr + pack->offset;
    r = tkl_flash_write(write_addr, pack->data, pack->len);
    if (r != OPRT_OK) {
        return r;
    }

    /* Optional read-back verification. */
    /* A minimal verification can be added here (read into temp buffer and compare)
     * if your platform requires extra safety. */

    *remain_len = 0u;
    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_ota_end_notify(BOOL_T reset)
{
    // --- BEGIN: user implements ---
    if (!g_ota.active) {
        return OPRT_COM_ERROR;
    }

    /* Finalization step placeholder:
     * - Verify image integrity (CRC/MD5) if required by your platform
     * - Mark image as pending/valid for bootloader (AB switch) if applicable
     */

    g_ota.active = false;

    if (reset) {
        /* If you have tkl_system_reset() available, you can call it here.
         * Keeping this function side-effect free if reset is handled elsewhere. */
        /* extern void tkl_system_reset(void);
           tkl_system_reset(); */
    }

    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_ota_get_old_firmware_info(TUYA_OTA_FIRMWARE_INFO_T **info)
{
    // --- BEGIN: user implements ---
    if (info == NULL) {
        return OPRT_INVALID_PARM;
    }

    /* Not implemented: used for breakpoint resume on specific sub-device scenarios. */
    *info = NULL;
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}
