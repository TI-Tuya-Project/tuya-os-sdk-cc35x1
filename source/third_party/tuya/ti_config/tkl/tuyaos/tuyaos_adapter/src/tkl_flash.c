#include "tkl_flash.h"
#include "tuya_error_code.h"
#include "tkl_mutex.h"  // Added for synchronization
#include <string.h>

/* TI SimpleLink Symbols */
extern const uint32_t nvocmp_physical_slot_address;
extern const uint32_t nvocmp_logical_slot_address;
extern const uint32_t nvocmp_region_size;

typedef void* XMEM_Handle;
typedef struct {
    uint32_t regionBase;
    uint32_t regionStartAddr;
    uint32_t regionSize;
    uint32_t deviceNum;
} XMEM_Params;

extern void XMEMWFF3_init(void);
extern XMEM_Handle XMEMWFF3_open(XMEM_Params *params);
extern int_fast16_t XMEMWFF3_read(XMEM_Handle handle, uint32_t offset, uint8_t *buffer, size_t bufferSize, uint32_t flags);
extern int_fast16_t XMEMWFF3_write(XMEM_Handle handle, uint32_t offset, uint8_t *buffer, size_t bufferSize, uint32_t flags);
extern int_fast16_t XMEMWFF3_erase(XMEM_Handle handle, uint32_t offset, size_t size);

static XMEM_Handle tuya_xmem_handle = NULL;
static TKL_MUTEX_HANDLE flash_mutex = NULL; // The "Traffic Cop" for the Flash Bus

static OPERATE_RET _ensure_flash_open(void) {
    // 1. Initialize the Mutex first if it doesn't exist
    if (flash_mutex == NULL) {
        tkl_mutex_create_init(&flash_mutex);
    }

    if (tuya_xmem_handle != NULL) return OPRT_OK;

    if (nvocmp_region_size == 0) {
        return OPRT_COM_ERROR;
    }

    XMEMWFF3_init();
    
    static XMEM_Params attrs;
    attrs.regionBase      = nvocmp_physical_slot_address;
    attrs.regionStartAddr = nvocmp_logical_slot_address;
    attrs.regionSize      = nvocmp_region_size;
    attrs.deviceNum       = 0;

    tuya_xmem_handle = XMEMWFF3_open(&attrs);
    
    return (tuya_xmem_handle == NULL) ? OPRT_COM_ERROR : OPRT_OK;
}

/* --- LOCKING MECHANISM --- */
/* Tuya KV storage calls these automatically before every operation */

OPERATE_RET tkl_flash_lock(uint32_t addr, uint32_t size) {
    if (_ensure_flash_open() != OPRT_OK) return OPRT_COM_ERROR;
    if (flash_mutex) {
        tkl_mutex_lock(flash_mutex); // KV or Test waits here if the other is busy
    }
    return OPRT_OK;
}

OPERATE_RET tkl_flash_unlock(uint32_t addr, uint32_t size) {
    if (flash_mutex) {
        tkl_mutex_unlock(flash_mutex);
    }
    return OPRT_OK;
}

/* --- WRAPPED OPERATIONS --- */

OPERATE_RET tkl_flash_read(uint32_t addr, uint8_t *dst, uint32_t size) {
    if (dst == NULL) return OPRT_INVALID_PARM;
    if (_ensure_flash_open() != OPRT_OK) return OPRT_COM_ERROR;
    
    int_fast16_t res = XMEMWFF3_read(tuya_xmem_handle, addr, dst, size, 0);
    return (res >= 0) ? OPRT_OK : OPRT_COM_ERROR;
}

OPERATE_RET tkl_flash_write(uint32_t addr, const uint8_t *src, uint32_t size) {
    if (src == NULL) return OPRT_INVALID_PARM;
    if (_ensure_flash_open() != OPRT_OK) return OPRT_COM_ERROR;

    int_fast16_t res = XMEMWFF3_write(tuya_xmem_handle, addr, (uint8_t *)src, size, 0);
    return (res >= 0) ? OPRT_OK : OPRT_COM_ERROR;
}

OPERATE_RET tkl_flash_erase(uint32_t addr, uint32_t size) {
    if (_ensure_flash_open() != OPRT_OK) return OPRT_COM_ERROR;
    
    int_fast16_t res = XMEMWFF3_erase(tuya_xmem_handle, addr, size);
    return (res >= 0) ? OPRT_OK : OPRT_COM_ERROR;
}

OPERATE_RET tkl_flash_get_one_type_info(TUYA_FLASH_TYPE_E type, TUYA_FLASH_BASE_INFO_T *info) {
    if (info == NULL) return OPRT_INVALID_PARM;
    if (_ensure_flash_open() != OPRT_OK) return OPRT_COM_ERROR;

    info->partition_num = 1;
    info->partition[0].block_size = 4096;
    info->partition[0].start_addr = 0; 
    info->partition[0].size       = nvocmp_region_size;
    return OPRT_OK;
}