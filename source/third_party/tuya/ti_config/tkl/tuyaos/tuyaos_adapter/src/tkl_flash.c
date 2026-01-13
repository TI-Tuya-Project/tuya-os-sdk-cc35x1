/**
 * @file tkl_flash.c
 * @brief Flash driver implementation for TI CC35xx using NVS
 * @note This file maps Tuya Kernel Layer (TKL) flash operations to TI's NVS driver.
 *
 * @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
 */

// --- BEGIN: user defines and implements ---
#include "tkl_flash.h"
#include "tuya_error_code.h"
#include <ti/drivers/NVS.h>
#include <string.h>
#include <stdbool.h>

/* * NVS Index 0 corresponds to CONFIG_NVS_0 defined in the SysConfig.
 * Ensure you have added an NVS instance in your .syscfg file.
 */
static uint8_t TUYA_NVS_INDEX = 0;

static NVS_Handle nvsHandle = NULL;
static NVS_Attrs regionAttrs;

/**
 * @brief Helper function to ensure the NVS driver is open before any operation.
 * This implements "lazy initialization" to avoid dependency issues during startup.
 */
static OPERATE_RET _ensure_nvs_open(void) {
    if (nvsHandle != NULL) {
        return OPRT_OK;
    }

    NVS_init();
    
    // Open NVS using the determined index (usually 0)
    nvsHandle = NVS_open(TUYA_NVS_INDEX, NULL);

    if (nvsHandle == NULL) {
        return OPRT_COM_ERROR;
    }

    // Retrieve flash region attributes (sector size, region size)
    NVS_getAttrs(nvsHandle, &regionAttrs);
    return OPRT_OK;
}
// --- END: user defines and implements ---

/**
 * @brief read flash
 *
 * @param[in] addr: flash address (offset from the start of the NVS region)
 * @param[out] dst: pointer of buffer
 * @param[in] size: size of buffer
 *
 * @return OPRT_OK on success. Others on error.
 */
OPERATE_RET tkl_flash_read(uint32_t addr, uint8_t *dst, uint32_t size)
{
    // --- BEGIN: user implements ---
    if (_ensure_nvs_open() != OPRT_OK) {
        return OPRT_COM_ERROR;
    }

    // NVS_read takes an offset relative to the region start, not a physical address.
    int_fast16_t status = NVS_read(nvsHandle, addr, dst, size);

    if (status != NVS_STATUS_SUCCESS) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief write flash
 *
 * @param[in] addr: flash address (offset)
 * @param[in] src: pointer of buffer
 * @param[in] size: size of buffer
 *
 * @return OPRT_OK on success. Others on error.
 */
OPERATE_RET tkl_flash_write(uint32_t addr, const uint8_t *src, uint32_t size)
{
    // --- BEGIN: user implements ---
    if (_ensure_nvs_open() != OPRT_OK) {
        return OPRT_COM_ERROR;
    }

    // NVS_write handles programming. 
    // NVS_WRITE_POST_VERIFY ensures data is written correctly.
    int_fast16_t status = NVS_write(nvsHandle, addr, (void *)src, size, NVS_WRITE_POST_VERIFY);

    if (status != NVS_STATUS_SUCCESS) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief erase flash
 *
 * @param[in] addr: flash address (offset)
 * @param[in] size: size of flash block
 *
 * @return OPRT_OK on success. Others on error.
 */
OPERATE_RET tkl_flash_erase(uint32_t addr, uint32_t size)
{
    // --- BEGIN: user implements ---
    if (_ensure_nvs_open() != OPRT_OK) {
        return OPRT_COM_ERROR;
    }

    int_fast16_t status = NVS_erase(nvsHandle, addr, size);

    if (status != NVS_STATUS_SUCCESS) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief lock flash
 * @note TI NVS driver manages locking internally or via hardware configuration.
 * This function returns OK to satisfy the TKL API requirements.
 */
OPERATE_RET tkl_flash_lock(uint32_t addr, uint32_t size)
{
    // --- BEGIN: user implements ---
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief unlock flash
 * @note TI NVS driver manages locking internally or via hardware configuration.
 * This function returns OK to satisfy the TKL API requirements.
 */
OPERATE_RET tkl_flash_unlock(uint32_t addr, uint32_t size)
{
    // --- BEGIN: user implements ---
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief get flash information
 *
 * @param[out] info: the description of the flash
 *
 * @return OPRT_OK on success. Others on error.
 */
OPERATE_RET tkl_flash_get_one_type_info(TUYA_FLASH_TYPE_E type, TUYA_FLASH_BASE_INFO_T *info)
{
    // --- BEGIN: user implements ---
    if (info == NULL) {
        return OPRT_INVALID_PARM;
    }

    if (_ensure_nvs_open() != OPRT_OK) {
        return OPRT_COM_ERROR;
    }

    // Map the NVS region to a single partition.
    info->partition_num = 1;
    info->partition[0].block_size = regionAttrs.sectorSize;
    
    // Currently mapping all types to the start of the NVS region (Offset 0).
    info->partition[0].start_addr = 0; 
    info->partition[0].size       = regionAttrs.regionSize;

    return OPRT_OK;
    // --- END: user implements ---
}