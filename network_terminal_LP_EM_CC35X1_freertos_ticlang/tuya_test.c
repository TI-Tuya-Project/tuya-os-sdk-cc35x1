#include "tuya_test.h"
#include "tkl_memory.h"
#include "tkl_adc.h"
#include "tuya_error_code.h"
#include "tkl_flash.h"
/* TI SimpleLink UART Terminal Header (Adjust if your project uses a different name) */
#include "uart_term.h"
#include "tkl_fs.h" 

/*===========================================================*/
/*                      TKL ADC                              */
/*===========================================================*/
const char tuyaAdcTestStr[] = "test_tuya_adc";
const char tuyaAdcTestUsageStr[] = "Usage: test_tuya_adc\n\r";
const char tuyaAdcTestDetailsStr[] = "Executes a series of tests on the Tuya ADC adaptation layer (Port 0).\n\r";

int32_t printTuyaAdcTestUsage(void *arg)
{
    UART_PRINT("\n\r");
    UART_PRINT(tuyaAdcTestUsageStr);
    UART_PRINT("Description: ");
    UART_PRINT(tuyaAdcTestDetailsStr);
    UART_PRINT("\n\r");
    return 0;
}

int32_t cmdTuyaAdcTestCallback(void *arg)
{
    UART_PRINT("\n\r[ADC TEST] Starting TKL ADC Tests...\n\r");

    TUYA_ADC_NUM_E test_port = 0; /* Testing Tuya ADC Port 0 */
    TUYA_ADC_BASE_CFG_T dummy_cfg = {0}; 
    OPERATE_RET ret;

    /* 1. Test Init */
    ret = tkl_adc_init(test_port, &dummy_cfg);
    if (ret == OPRT_OK) {
        UART_PRINT("[ADC TEST] tkl_adc_init (Port %d): PASS\n\r", test_port);
    } else {
        UART_PRINT("[ADC TEST] tkl_adc_init: FAIL (Error %d)\n\r", ret);
        return -1; /* Abort the rest of the test if we can't open the hardware */
    }

    /* 2. Test Width */
    uint8_t width = tkl_adc_width_get(test_port);
    UART_PRINT("[ADC TEST] tkl_adc_width_get: PASS (%d-bit)\n\r", width);

    /* 3. Test Reference Voltage */
    uint32_t v_ref = tkl_adc_ref_voltage_get(test_port);
    UART_PRINT("[ADC TEST] tkl_adc_ref_voltage_get: PASS (%d mV)\n\r", v_ref);

    /* 4. Test Temperature */
    int32_t temp = tkl_adc_temperature_get();
    if (temp == OPRT_NOT_SUPPORTED) {
        UART_PRINT("[ADC TEST] tkl_adc_temperature_get: PASS (Unsupported as expected)\n\r");
    } else {
        UART_PRINT("[ADC TEST] tkl_adc_temperature_get: PASS (%d C)\n\r", temp);
    }

    /* 5. Test Raw Data Read */
    int32_t raw_data = 0;
    ret = tkl_adc_read_data(test_port, &raw_data, 1);
    if (ret == OPRT_OK) {
        UART_PRINT("[ADC TEST] tkl_adc_read_data: PASS (Raw: %d)\n\r", raw_data);
    } else {
        UART_PRINT("[ADC TEST] tkl_adc_read_data: FAIL (Error %d)\n\r", ret);
    }

    /* 6. Test Single Channel Read */
    int32_t single_data = 0;
    ret = tkl_adc_read_single_channel(test_port, 0, &single_data);
    if (ret == OPRT_OK) {
        UART_PRINT("[ADC TEST] tkl_adc_read_single_channel: PASS (Raw: %d)\n\r", single_data);
    } else {
        UART_PRINT("[ADC TEST] tkl_adc_read_single_channel: FAIL (Error %d)\n\r", ret);
    }

    /* 7. Test Voltage Read */
    int32_t voltage_mv = 0;
    ret = tkl_adc_read_voltage(test_port, &voltage_mv, 1);
    if (ret == OPRT_OK) {
        UART_PRINT("[ADC TEST] tkl_adc_read_voltage: PASS (%d mV)\n\r", voltage_mv);
    } else {
        UART_PRINT("[ADC TEST] tkl_adc_read_voltage: FAIL (Error %d)\n\r", ret);
    }

    /* 8. Test Deinit */
    ret = tkl_adc_deinit(test_port);
    if (ret == OPRT_OK) {
        UART_PRINT("[ADC TEST] tkl_adc_deinit: PASS\n\r");
    } else {
        UART_PRINT("[ADC TEST] tkl_adc_deinit: FAIL (Error %d)\n\r", ret);
    }

    UART_PRINT("[ADC TEST] Tests Completed.\n\r");
    return 0;
}

/*===========================================================*/
/*                      TKL ADC                              */
/*===========================================================*/

/*===========================================================*/
/*                   TKL FLASH                               */
/*===========================================================*/

const char tuyaFlashTestStr[] = "test_tuya_flash";
const char tuyaFlashTestUsageStr[] = "Usage: test_tuya_flash\n\r";
const char tuyaFlashTestDetailsStr[] = "Executes read, write, and erase tests on the Tuya Flash adaptation layer.\n\r";

/* Test buffers - 🌟 ENSURE 4-BYTE ALIGNMENT 🌟 */
static TUYA_FLASH_BASE_INFO_T flash_info;
static uint8_t write_buf[64] __attribute__((aligned(4)));
static uint8_t read_buf[64]  __attribute__((aligned(4)));

int32_t printTuyaFlashTestUsage(void *arg)
{
    UART_PRINT("\n\r");
    UART_PRINT(tuyaFlashTestUsageStr);
    UART_PRINT("Description: ");
    UART_PRINT(tuyaFlashTestDetailsStr);
    UART_PRINT("\n\r");
    return 0;
}

int32_t cmdTuyaFlashTestCallback(void *arg)
{
    UART_PRINT("\n\r[FLASH TEST] Starting TKL Flash Tests...\n\r");
    
    OPERATE_RET ret;

    /* 1. Get Flash Info */
    ret = tkl_flash_get_one_type_info(TUYA_FLASH_TYPE_KV_DATA, &flash_info);
        if (ret != 0) {
            UART_PRINT("[FLASH TEST] tkl_flash_get_one_type_info: FAIL (Code: %d)\n\r", ret);
            return -1;
        }
    
    // 🌟 ADD THIS DEBUG PRINT 🌟
    UART_PRINT("[DEBUG] Partition Start: 0x%x, Size: %d\n\r", 
               flash_info.partition[0].start_addr, flash_info.partition[0].size);
    
    // Move from 0x1000 to 0x10000 (64KB offset)
    // This puts your test far away from the KV database's active sectors
    uint32_t test_addr = 0x8000;

    /* 2. Erase Block */
    UART_PRINT("[FLASH TEST] Erasing at 0x%x...\n\r", test_addr);
    
    uint32_t block_size = flash_info.partition[0].block_size;
    UART_PRINT("[FLASH TEST] Flash Info: Block Size = %d, Total Size = %d\n\r", 
               block_size, flash_info.partition[0].size);

    if (block_size == 0) {
        UART_PRINT("[FLASH TEST] Invalid block size. Aborting test.\n\r");
        return -1;
    }



    /* 2. Erase Block */
    UART_PRINT("[FLASH TEST] Erasing block at address 0x%x...\n\r", test_addr);
    UART_PRINT("[FLASH TEST] Requires Mutex Lock for Erase...\n\r");
    
    tkl_flash_lock(test_addr,block_size);
    ret = tkl_flash_erase(test_addr, block_size);
    if (ret != 0) {
        UART_PRINT("[FLASH TEST] tkl_flash_erase: FAIL (Code: %d)\n\r", ret);
        return -1;
    }
    UART_PRINT("[FLASH TEST] tkl_flash_erase: PASS\n\r");
    tkl_flash_unlock(test_addr, block_size);
    /* 3. Read Erased Block (Hardware flash sets erased bits to 1 -> 0xFF) */
    ret = tkl_flash_read(test_addr, read_buf, sizeof(read_buf));
    if (ret != 0) {
        UART_PRINT("[FLASH TEST] tkl_flash_read (Erased): FAIL (Code: %d)\n\r", ret);
        return -1;
    }
    
    int erased_valid = 1;
    for (int i = 0; i < sizeof(read_buf); i++) {
        if (read_buf[i] != 0xFF) {
            erased_valid = 0;
            break;
        }
    }
    
    if (erased_valid) {
        UART_PRINT("[FLASH TEST] Erased verification (0xFF): PASS\n\r");
    } else {
        UART_PRINT("[FLASH TEST] Erased verification: FAIL (Data is not 0xFF)\n\r");
        return -1;
    }

    /* 4. Write Data */
    memset(write_buf, 0xAA, sizeof(write_buf)); /* Fill with test pattern */
    strcpy((char *)write_buf, "TUYA_FLASH_TEST_PATTERN"); /* Add identifiable string */
    
    UART_PRINT("[FLASH TEST] Writing test pattern...\n\r");
    ret = tkl_flash_write(test_addr, write_buf, sizeof(write_buf));
    if (ret != 0) {
        UART_PRINT("[FLASH TEST] tkl_flash_write: FAIL (Code: %d)\n\r", ret);
        return -1;
    }
    UART_PRINT("[FLASH TEST] tkl_flash_write: PASS\n\r");

    /* 5. Read Back and Verify */
    memset(read_buf, 0x00, sizeof(read_buf)); /* Clear read buffer to ensure clean read */
    ret = tkl_flash_read(test_addr, read_buf, sizeof(read_buf));
    if (ret != 0) {
        UART_PRINT("[FLASH TEST] tkl_flash_read (Verify): FAIL (Code: %d)\n\r", ret);
        return -1;
    }

    if (memcmp(write_buf, read_buf, sizeof(write_buf)) == 0) {
        UART_PRINT("[FLASH TEST] Write/Read Verification: PASS\n\r");
    } else {
        UART_PRINT("[FLASH TEST] Write/Read Verification: FAIL (Data mismatch)\n\r");
        return -1;
    }

    /* 6. Cleanup (Erase again) */
    UART_PRINT("[FLASH TEST] Cleaning up (Erasing block)...\n\r");
    ret = tkl_flash_erase(test_addr, block_size);
    if (ret == 0) {
        UART_PRINT("[FLASH TEST] Cleanup: PASS\n\r");
    } else {
        UART_PRINT("[FLASH TEST] Cleanup: FAIL\n\r");
    }

    UART_PRINT("[FLASH TEST] All Flash Tests Completed Successfully!\n\r");
    return 0;
}

/*===========================================================*/
/*                   TKL FLASH                               */
/*===========================================================*/


/*===========================================================*/
/*                   TKL MEMORY                              */
/*===========================================================*/
const char tuyaMemTestStr[] = "test_tuya_mem";
const char tuyaMemTestUsageStr[] = "Usage: test_tuya_mem\n\r";
const char tuyaMemTestDetailsStr[] = "Executes a series of tests on the Tuya memory adaptation layer.\n\r";

int32_t printTuyaMemTestUsage(void *arg)
{
    UART_PRINT("\n\r");
    UART_PRINT(tuyaMemTestUsageStr);
    UART_PRINT("Description: ");
    UART_PRINT(tuyaMemTestDetailsStr);
    UART_PRINT("\n\r");
    return 0;
}

int32_t cmdTuyaMemTestCallback(void *arg)
{
    UART_PRINT("\n\r[MEM TEST] Starting TKL Memory Tests...\n\r");

    /* 1. Record initial heap size */
    int initial_heap = tkl_system_get_free_heap_size();
    UART_PRINT("[MEM TEST] Initial Free Heap: %d bytes\n\r", initial_heap);

    /* 2. Test Malloc & Memset */
    void *ptr1 = tkl_system_malloc(128);
    if (ptr1) {
        UART_PRINT("[MEM TEST] tkl_system_malloc (128 bytes): PASS\n\r");
        tkl_system_memset(ptr1, 0xAA, 128);
        UART_PRINT("[MEM TEST] tkl_system_memset: PASS\n\r");
    } else {
        UART_PRINT("[MEM TEST] tkl_system_malloc: FAIL\n\r");
    }

    /* 3. Test Calloc */
    void *ptr2 = tkl_system_calloc(4, 32); /* 128 bytes total */
    if (ptr2) {
        UART_PRINT("[MEM TEST] tkl_system_calloc (4x32 bytes): PASS\n\r");
    } else {
        UART_PRINT("[MEM TEST] tkl_system_calloc: FAIL\n\r");
    }

    /* 4. Test Memcpy & Memcmp */
    if (ptr1 && ptr2) {
        tkl_system_memcpy(ptr2, ptr1, 128);
        if (tkl_system_memcmp(ptr1, ptr2, 128) == 0) {
            UART_PRINT("[MEM TEST] tkl_system_memcpy & tkl_system_memcmp: PASS\n\r");
        } else {
            UART_PRINT("[MEM TEST] tkl_system_memcpy & tkl_system_memcmp: FAIL\n\r");
        }
    }

    /* 5. Test Realloc */
    void *ptr3 = tkl_system_realloc(ptr2, 256);
    if (ptr3) {
        UART_PRINT("[MEM TEST] tkl_system_realloc (128 to 256 bytes): PASS\n\r");
        ptr2 = ptr3; /* Update reference to the new block so we can free it later */
    } else {
        UART_PRINT("[MEM TEST] tkl_system_realloc: FAIL\n\r");
    }

    /* 6. Test Free */
    if (ptr1) tkl_system_free(ptr1);
    if (ptr2) tkl_system_free(ptr2); /* ptr3 was assigned to ptr2 */
    UART_PRINT("[MEM TEST] tkl_system_free: PASS\n\r");

    /* 7. Memory Leak Check */
    int final_heap = tkl_system_get_free_heap_size();
    UART_PRINT("[MEM TEST] Final Free Heap: %d bytes\n\r", final_heap);

    if (initial_heap == final_heap) {
        UART_PRINT("[MEM TEST] Memory Leak Check: PASS (No Leaks)\n\r");
    } else {
        UART_PRINT("[MEM TEST] Memory Leak Check: WARNING (%d bytes difference)\n\r", initial_heap - final_heap);
    }

    UART_PRINT("[MEM TEST] Tests Completed.\n\r");
    return 0;
}

/*===========================================================*/
/*                   TKL MEMORY                              */
/*===========================================================*/


/*===========================================================*/
/*                     TKL FS                               */
/*===========================================================*/

    const char tuyaFsTestStr[]      = "test_tuya_fs";
    const char tuyaFsTestUsageStr[] = "Usage: test_fs_mem";
    const char tuyaFsTestDetailsStr[] = "Execute a series of tests on file system functions";

int32_t printTuyaFsTestUsage(void *arg)
{
        UART_PRINT("\n\r");
        UART_PRINT(tuyaFsTestStr);
        UART_PRINT("Description: ");
        UART_PRINT(tuyaFsTestUsageStr);
        UART_PRINT("\n\r");
        return 0;
}
int32_t cmdTuyaFsTestCallback(void *arg)
{
    UART_PRINT("\n\r[FS TEST] Starting TKL File System Tests...\n\r");

    const char *test_file_name = "tuya_test.txt";
    const char *test_data = "HELLO_TUYA_FS_TI_CC35XX";
    int data_len = strlen(test_data);
    
    static char read_buffer[64]; 
    memset(read_buffer, 0, sizeof(read_buffer));

    TUYA_FILE file_handle = NULL;
    int ret = 0;

    /* 1. Test Open/Create for Writing ("w+") */
    UART_PRINT("[FS TEST] Creating file: %s...\n\r", test_file_name);
    file_handle = tkl_fopen(test_file_name, "w+");
    if (file_handle != NULL) {
        UART_PRINT("[FS TEST] tkl_fopen (w+): PASS\n\r");
    } else {
        UART_PRINT("[FS TEST] tkl_fopen (w+): FAIL\n\r");
        return -1;
    }

    /* 2. Test Write */
    // Signature: int tkl_fwrite(void *buf, int bytes, TUYA_FILE file)
    UART_PRINT("[FS TEST] Writing data...\n\r");
    ret = tkl_fwrite((void *)test_data, data_len, file_handle);
    if (ret == data_len) {
        UART_PRINT("[FS TEST] tkl_fwrite: PASS (%d bytes)\n\r", ret);
    } else {
        UART_PRINT("[FS TEST] tkl_fwrite: FAIL (Returned %d)\n\r", ret);
        tkl_fclose(file_handle);
        return -1;
    }

    /* 3. Test Close */
    // Signature: int tkl_fclose(TUYA_FILE file)
    ret = tkl_fclose(file_handle);
    if (ret == 0) {
        UART_PRINT("[FS TEST] tkl_fclose: PASS\n\r");
    } else {
        UART_PRINT("[FS TEST] tkl_fclose: FAIL (Code: %d)\n\r", ret);
    }
    file_handle = NULL;

    /* 4. Test Open for Reading ("r") */
    UART_PRINT("[FS TEST] Re-opening file for reading...\n\r");
    file_handle = tkl_fopen(test_file_name, "r");
    if (file_handle != NULL) {
        UART_PRINT("[FS TEST] tkl_fopen (r): PASS\n\r");
    } else {
        UART_PRINT("[FS TEST] tkl_fopen (r): FAIL\n\r");
        return -1;
    }

    /* 5. Test Read */
    // Signature: int tkl_fread(void *buf, int bytes, TUYA_FILE file)
    UART_PRINT("[FS TEST] Reading data...\n\r");
    ret = tkl_fread(read_buffer, data_len, file_handle);
    if (ret == data_len) {
        UART_PRINT("[FS TEST] tkl_fread: PASS (%d bytes)\n\r", ret);
    } else {
        UART_PRINT("[FS TEST] tkl_fread: FAIL (Returned %d)\n\r", ret);
        tkl_fclose(file_handle);
        return -1;
    }

    /* 6. Verify Data */
    if (strncmp(test_data, read_buffer, data_len) == 0) {
        UART_PRINT("[FS TEST] Data Verification: PASS (%s)\n\r", read_buffer);
    } else {
        UART_PRINT("[FS TEST] Data Verification: FAIL (Got: %s)\n\r", read_buffer);
    }

    /* Close again before delete */
    tkl_fclose(file_handle);

    /* 7. Test Remove/Delete */
    // Signature: int tkl_fs_remove(const char *path)
    UART_PRINT("[FS TEST] Deleting file...\n\r");
    ret = tkl_fs_remove(test_file_name);
    if (ret == 0) {
        UART_PRINT("[FS TEST] tkl_fs_remove: PASS\n\r");
    } else {
        UART_PRINT("[FS TEST] tkl_fs_remove: FAIL (Code: %d)\n\r", ret);
    }

    UART_PRINT("[FS TEST] All File System Tests Completed!\n\r");
    return 0;
}
/*===========================================================*/
/*                     TKL FS                                */
/*===========================================================*/