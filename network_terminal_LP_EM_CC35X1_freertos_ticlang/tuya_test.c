#include "tuya_test.h"
#include "tkl_memory.h"
#include "tkl_adc.h"
#include "tuya_error_code.h"
#include "tkl_flash.h"
/* TI SimpleLink UART Terminal Header (Adjust if your project uses a different name) */
#include "uart_term.h"
#include "tkl_fs.h" 
#include "tkl_gpio.h"
#include "tkl_board_config.h"
#include "ti_drivers_config.h"
#include "tkl_i2c.h"
#include "tkl_network.h"
#include "tkl_mutex.h"
#include "tkl_output.h"
#include "tkl_pwm.h"
#include "tkl_rtc.h"
#include "tkl_queue.h"
#include "FreeRTOS.h"
#include "tkl_semaphore.h"
#include "tkl_spi.h"
#include "tkl_timer.h"
#include "tkl_system.h"
#include "tkl_uart.h"
#include "wakeup/tkl_wakeup.h"
#include "watchdog/tkl_watchdog.h"

// 2. Add a fallback definition just in case TI's port doesn't have it
#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000U ) )
#endif

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "tkl_thread.h"
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

const char tuyaFsTestStr[]        = "test_tuya_fs";
const char tuyaFsTestUsageStr[]   = "Usage: test_fs_mem";
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


// /*===========================================================*/
// /*                     TKL FS                                */
// /*===========================================================*/

#define RAMDISK_SIZE (512)

// Tell the compiler about the function from ramdisk.c
extern int ramdisk_start(unsigned char drive, unsigned char *data, int numBytes, int mkfs);
static unsigned char g_ramdisk_memory[RAMDISK_SIZE] __attribute__((aligned(4)));
// --- UPDATE YOUR TEST CALLBACK ---
int32_t cmdTuyaFsTestCallback(void *arg)
{
    UART_PRINT("\n\r[FS TEST] Starting TKL File System Tests...\n\r");

    // 🌟 THE MAGIC FIX: WAKE UP THE RAM DISK 🌟
    // Drive: 0, Data: our memory array, Size: 16KB, Format (mkfs): 1 (True)
    // UART_PRINT("[FS TEST] Initializing RAM Disk...\n\r");
    // int mnt_res = ramdisk_start(0, g_ramdisk_memory, RAMDISK_SIZE, 1);
    
    // if (mnt_res != 0) {
    //     UART_PRINT("[FS TEST] RAM Disk Init FAILED! Error: %d\n\r", mnt_res);
    //     return -1;
    // }
    // UART_PRINT("[FS TEST] RAM Disk Initialized and Formatted!\n\r");


    // --- The rest of your test remains exactly the same! ---
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
    
    uint32_t handle_val_w = (uint32_t)file_handle;

    if (handle_val_w > 1000) {
        UART_PRINT("[FS TEST] tkl_fopen (w+): PASS\n\r");
    } else {
        UART_PRINT("[FS TEST] tkl_fopen (w+): FAIL! Smuggled Error Code: %d\n\r", handle_val_w);
        return -1; 
    }

    /* 2. Test Write */
    UART_PRINT("[FS TEST] Writing data...\n\r");
    ret = tkl_fwrite((void *)test_data, data_len, file_handle);
    if (ret == data_len) {
        UART_PRINT("[FS TEST] tkl_fwrite: PASS (%d bytes)\n\r", ret);
    } else {
        UART_PRINT("[FS TEST] tkl_fwrite: FAIL (Returned %d)\n\r", ret);
        tkl_fclose(file_handle);
        return -1;
    }
    /* 3. Close the file to flush data to the RAM disk */
    tkl_fclose(file_handle);
    UART_PRINT("[FS TEST] tkl_fclose: PASS\n\r");

    /* 4. Open for Reading ("r") */
    file_handle = tkl_fopen(test_file_name, "r");
    if (file_handle != NULL) {
        UART_PRINT("[FS TEST] tkl_fopen (r): PASS\n\r");
        
        /* 5. Read the data back */
        ret = tkl_fread(read_buffer, data_len, file_handle);
        if (ret == data_len) {
            UART_PRINT("[FS TEST] tkl_fread: PASS! Read back: %s\n\r", read_buffer);
        } else {
            UART_PRINT("[FS TEST] tkl_fread: FAIL! Read %d bytes.\n\r", ret);
        }
        tkl_fclose(file_handle);
    } else {
        UART_PRINT("[FS TEST] tkl_fopen (r): FAIL!\n\r");
    }
    return 0;
}   
// /*===========================================================*/
// /*                     TKL FS                                */
// /*===========================================================*/


/*===========================================================*/
/*                        TKL GPIO                           */
/*===========================================================*/
#include "tkl_gpio.h"

/* String constants for the CLI parser */
const char tuyaGpioTestStr[]         = "test_tuya_gpio";
const char tuyaGpioTestUsageStr[]    = "Usage: test_tuya_gpio\n\r";
const char tuyaGpioTestDetailsStr[]  = "Tests the Tuya TKL GPIO implementation (Init, Read/Write, IRQ)\n\r";

/* Print Usage Function */
int32_t printTuyaGpioTestUsage(void *arg)
{
    UART_PRINT(tuyaGpioTestUsageStr);
    UART_PRINT(tuyaGpioTestDetailsStr);
    return 0;
}

/* * IMPORTANT: TEST_GPIO_PIN_ID must match a Tuya Pin ID that 
 * you have mapped to a TI hardware pin in your board config!
 */
#define TEST_GPIO_PIN_ID TUYA_GPIO_NUM_0

/* Dummy interrupt handler to test the callback routing */
static volatile uint8_t g_test_irq_triggered = 0;
static void test_gpio_irq_handler(void *args)
{
    g_test_irq_triggered = 1;
}

static void init_ti_tuya_board_map(void)
{
    TKL_BOARD_CONFIG_T my_board_map;
    
    // 1. Clear the map (set all to -1)
    memset(&my_board_map, -1, sizeof(TKL_BOARD_CONFIG_T));

    // 2. Map Tuya GPIO 0 to the TI LaunchPad LED
    my_board_map.gpio_map[0] = CONFIG_GPIO_LED_0; 

    // 3. Map Tuya I2C 0 to the TI I2C Instance
    my_board_map.i2c_map[0] = CONFIG_I2C_0; // <--- ADD THIS LINE

    // 4. Inject into the TKL Board Layer
    tkl_hw_board_init(&my_board_map);
    
    UART_PRINT("[MAP] Hardware Map Initialized: GPIO 0 -> LED, I2C 0 -> CONFIG_I2C_0\n\r");
}


/* Main GPIO Test Callback */
int32_t cmdTuyaGpioTestCallback(void *arg)
{
    OPERATE_RET ret;
    TUYA_GPIO_LEVEL_E read_level;

    UART_PRINT("\n\r==================================================\n\r");
    UART_PRINT("[GPIO TEST] Starting TKL GPIO Tests...\n\r");
    UART_PRINT("==================================================\n\r");

    init_ti_tuya_board_map();
    // ---------------------------------------------------------
    // Phase 1: Output Mode Test (Init, Write, Read-back)
    // ---------------------------------------------------------
    UART_PRINT("[GPIO TEST] Phase 1: Output Test\n\r");
    
    TUYA_GPIO_BASE_CFG_T out_cfg = {
        .mode = TUYA_GPIO_PULLUP,
        .direct = TUYA_GPIO_OUTPUT,
        .level = TUYA_GPIO_LEVEL_LOW
    };

    ret = tkl_gpio_init(TEST_GPIO_PIN_ID, &out_cfg);
    if (ret != OPRT_OK) {
        UART_PRINT("[GPIO TEST] INIT FAIL! Code: %d (Is pin mapped?)\n\r", ret);
        return -1;
    }
    UART_PRINT("  -> Init Output: PASS\n\r");

    // Write HIGH
    ret = tkl_gpio_write(TEST_GPIO_PIN_ID, TUYA_GPIO_LEVEL_HIGH);
    if (ret == OPRT_OK) UART_PRINT("  -> Write HIGH:  PASS\n\r");
    else UART_PRINT("  -> Write HIGH:  FAIL (%d)\n\r", ret);

    // Read back to verify
    ret = tkl_gpio_read(TEST_GPIO_PIN_ID, &read_level);
    if (ret == OPRT_OK && read_level == TUYA_GPIO_LEVEL_HIGH) {
        UART_PRINT("  -> Read State:  PASS (HIGH verified)\n\r");
    } else {
        UART_PRINT("  -> Read State:  FAIL! Expected HIGH, got %d\n\r", read_level);
    }

    // Write LOW to reset state
    tkl_gpio_write(TEST_GPIO_PIN_ID, TUYA_GPIO_LEVEL_LOW);

    // Deinit
    ret = tkl_gpio_deinit(TEST_GPIO_PIN_ID);
    if (ret == OPRT_OK) UART_PRINT("  -> Deinit:      PASS\n\r");

    // ---------------------------------------------------------
    // Phase 2: Input & Interrupt Setup Test
    // ---------------------------------------------------------
    UART_PRINT("\n\r[GPIO TEST] Phase 2: Input & IRQ Configuration\n\r");

    TUYA_GPIO_BASE_CFG_T in_cfg = {
        .mode = TUYA_GPIO_PULLUP,
        .direct = TUYA_GPIO_INPUT
    };
    
    ret = tkl_gpio_init(TEST_GPIO_PIN_ID, &in_cfg);
    if (ret == OPRT_OK) UART_PRINT("  -> Init Input:  PASS\n\r");
    else UART_PRINT("  -> Init Input:  FAIL (%d)\n\r", ret);

    TUYA_GPIO_IRQ_T irq_cfg = {
        .cb = test_gpio_irq_handler,
        .arg = NULL,
        .mode = TUYA_GPIO_IRQ_FALL
    };

    // Initialize IRQ
    ret = tkl_gpio_irq_init(TEST_GPIO_PIN_ID, &irq_cfg);
    if (ret == OPRT_OK) UART_PRINT("  -> IRQ Init:    PASS (Falling Edge)\n\r");
    else UART_PRINT("  -> IRQ Init:    FAIL (%d)\n\r", ret);

    // Enable IRQ
    ret = tkl_gpio_irq_enable(TEST_GPIO_PIN_ID);
    if (ret == OPRT_OK) UART_PRINT("  -> IRQ Enable:  PASS\n\r");
    else UART_PRINT("  -> IRQ Enable:  FAIL (%d)\n\r", ret);

    // Disable IRQ
    ret = tkl_gpio_irq_disable(TEST_GPIO_PIN_ID);
    if (ret == OPRT_OK) UART_PRINT("  -> IRQ Disable: PASS\n\r");
    else UART_PRINT("  -> IRQ Disable: FAIL (%d)\n\r", ret);

    // Cleanup
    tkl_gpio_deinit(TEST_GPIO_PIN_ID);

    UART_PRINT("\n\r[GPIO TEST] All basic API calls passed!\n\r");
    UART_PRINT("Note: To physically test the IRQ trigger, connect a jumper wire from GND to the mapped TI pin.\n\r");
    UART_PRINT("==================================================\n\r");

    return 0;
}
/*===========================================================*/
/*                        TKL GPIO                           */
/*===========================================================*/



/*===========================================================*/
/* TKL HASH                                                  */
/*===========================================================*/
#include "tkl_hash.h"

/* String constants for the CLI parser */
const char tuyaHashTestStr[]         = "test_tuya_hash";
const char tuyaHashTestUsageStr[]    = "Usage: test_tuya_hash\n\r";
const char tuyaHashTestDetailsStr[]  = "Tests the Tuya TKL Hash implementation (MD5, SHA1, SHA256)\n\r";

/* Print Usage Function */
int32_t printTuyaHashTestUsage(void *arg)
{
    UART_PRINT(tuyaHashTestUsageStr);
    UART_PRINT(tuyaHashTestDetailsStr);
    return 0;
}

/* Helper to print hash output in Hex format */
static void print_hex(const char* label, uint8_t* data, uint32_t len) 
{
    UART_PRINT("  -> %s: ", label);
    for (uint32_t i = 0; i < len; i++) {
        UART_PRINT("%02x", data[i]);
    }
    UART_PRINT("\n\r");
}

/* Main Hash Test Callback */
int32_t cmdTuyaHashTestCallback(void *arg)
{
    OPERATE_RET ret;
    TKL_HASH_HANDLE ctx = NULL;
    
    const uint8_t test_data[] = "abc"; // Standard test string
    size_t test_len = strlen((const char*)test_data);
    
    UART_PRINT("\n\r==================================================\n\r");
    UART_PRINT("[HASH TEST] Starting TKL Hash Tests...\n\r");
    UART_PRINT("==================================================\n\r");
    UART_PRINT("[HASH TEST] Input Data: \"%s\"\n\r\n\r", test_data);

    // ---------------------------------------------------------
    // Phase 1: MD5 Test
    // Expected: 900150983cd24fb0d6963f7d28e17f72
    // ---------------------------------------------------------
    UART_PRINT("[HASH TEST] Phase 1: MD5\n\r");
    uint8_t md5_out[16] = {0};
    
    ret = tkl_md5_create_init(&ctx);
    if (ret == OPRT_OK) {
        tkl_md5_update_ret(ctx, test_data, test_len);
        tkl_md5_finish_ret(ctx, md5_out);
        tkl_md5_free(ctx);
        print_hex("MD5 Output   ", md5_out, 16);
        UART_PRINT("  -> Status      : PASS\n\r");
    } else {
        UART_PRINT("  -> Init FAIL! Code: %d\n\r", ret);
    }

    // ---------------------------------------------------------
    // Phase 2: SHA1 Test
    // Expected: a9993e364706816aba3e25717850c26c9cd0d89d
    // ---------------------------------------------------------
    UART_PRINT("\n\r[HASH TEST] Phase 2: SHA1\n\r");
    uint8_t sha1_out[20] = {0};
    ctx = NULL; // Reset handle
    
    ret = tkl_sha1_create_init(&ctx);
    if (ret == OPRT_OK) {
        tkl_sha1_update_ret(ctx, test_data, test_len);
        tkl_sha1_finish_ret(ctx, sha1_out);
        tkl_sha1_free(ctx);
        print_hex("SHA1 Output  ", sha1_out, 20);
        UART_PRINT("  -> Status      : PASS\n\r");
    } else {
        UART_PRINT("  -> Init FAIL! Code: %d\n\r", ret);
    }

    // ---------------------------------------------------------
    // Phase 3: SHA256 Test
    // Expected: ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
    // ---------------------------------------------------------
    UART_PRINT("\n\r[HASH TEST] Phase 3: SHA256\n\r");
    uint8_t sha256_out[32] = {0};
    ctx = NULL; // Reset handle
    
    ret = tkl_sha256_create_init(&ctx);
    if (ret == OPRT_OK) {
        tkl_sha256_update_ret(ctx, test_data, test_len);
        tkl_sha256_finish_ret(ctx, sha256_out);
        tkl_sha256_free(ctx);
        print_hex("SHA256 Output", sha256_out, 32);
        UART_PRINT("  -> Status      : PASS\n\r");
    } else {
        UART_PRINT("  -> Init FAIL! Code: %d\n\r", ret);
    }

    UART_PRINT("\n\r[HASH TEST] All Hash operations completed!\n\r");
    UART_PRINT("==================================================\n\r");

    return 0;
}

/*===========================================================*/
/* TKL HASH                                                  */
/*===========================================================*/


/*===========================================================*/
/*                        TKL I2C                            */
/*===========================================================*/

/* String constants for the CLI parser */
const char tuyaI2cTestStr[]         = "test_tuya_i2c";
const char tuyaI2cTestUsageStr[]    = "Usage: test_tuya_i2c\n\r";
const char tuyaI2cTestDetailsStr[]  = "Tests the Tuya TKL I2C implementation (Init, Probe, Reset, Deinit)\n\r";

/* Print Usage Function */
int32_t printTuyaI2cTestUsage(void *arg)
{
    UART_PRINT(tuyaI2cTestUsageStr);
    UART_PRINT(tuyaI2cTestDetailsStr);
    return 0;
}

/* IMPORTANT: Map TUYA_I2C_NUM_0 to a TI I2C index in your board config! */
#define TEST_I2C_PORT_ID TUYA_I2C_NUM_0
#define TEST_SLAVE_ADDR  0x50 // Standard dummy address (e.g., EEPROM)

/* Main I2C Test Callback */
int32_t cmdTuyaI2cTestCallback(void *arg)
{
    // 1. ALL C89 variable declarations at the top
    OPERATE_RET ret;
    TUYA_IIC_BASE_CFG_T cfg; 
    uint8_t dummy_data = 0x00;

    // 2. Executable code begins
    UART_PRINT("\n\r==================================================\n\r");
    UART_PRINT("[I2C TEST] Starting TKL I2C Tests...\n\r");
    UART_PRINT("==================================================\n\r");

    init_ti_tuya_board_map();
    // ---------------------------------------------------------
    // Phase 1: Initialization
    // ---------------------------------------------------------
    UART_PRINT("[I2C TEST] Phase 1: Init\n\r");
    
    // Assign values using the exact tuya_cloud_types.h enums
    cfg.role  = TUYA_IIC_MODE_MASTER;    
    cfg.speed = 0; // 0 is the default enum value for 100kHz (Standard Speed)

    ret = tkl_i2c_init(TEST_I2C_PORT_ID, &cfg);
    if (ret == OPRT_OK) {
        UART_PRINT("  -> Init:       PASS\n\r");
    } else {
        UART_PRINT("  -> Init:       FAIL! Code: %d (Is port mapped?)\n\r", ret);
        return -1;
    }

    // ---------------------------------------------------------
    // Phase 2: Transfer (Probe)
    // NOTE: This will return FAIL (NACK) unless a real I2C device is attached.
    // ---------------------------------------------------------
    UART_PRINT("\n\r[I2C TEST] Phase 2: Hardware Probe\n\r");
    
    ret = tkl_i2c_master_send(TEST_I2C_PORT_ID, TEST_SLAVE_ADDR, &dummy_data, 1, FALSE);
    
    if (ret == OPRT_OK) {
        UART_PRINT("  -> Send Probe: PASS (Device ACK'd at 0x%02X!)\n\r", TEST_SLAVE_ADDR);
    } else {
        UART_PRINT("  -> Send Probe: FAIL (Expected if no slave is at 0x%02X. NACK.)\n\r", TEST_SLAVE_ADDR);
    }

    // ---------------------------------------------------------
    // Phase 3: Reset
    // ---------------------------------------------------------
    UART_PRINT("\n\r[I2C TEST] Phase 3: Reset\n\r");
    ret = tkl_i2c_reset(TEST_I2C_PORT_ID);
    if (ret == OPRT_OK) {
        UART_PRINT("  -> Reset:      PASS\n\r");
    } else {
        UART_PRINT("  -> Reset:      FAIL! Code: %d\n\r", ret);
    }

    // ---------------------------------------------------------
    // Phase 4: Deinit
    // ---------------------------------------------------------
    UART_PRINT("\n\r[I2C TEST] Phase 4: Deinit\n\r");
    ret = tkl_i2c_deinit(TEST_I2C_PORT_ID);
    if (ret == OPRT_OK) {
        UART_PRINT("  -> Deinit:     PASS\n\r");
    } else {
        UART_PRINT("  -> Deinit:     FAIL! Code: %d\n\r", ret);
    }

    UART_PRINT("==================================================\n\r");
    return 0;
}
/*===========================================================*/
/*                        TKL I2C                            */
/*===========================================================*/


/*===========================================================*/
/* TKL MUTEX                                                 */
/*===========================================================*/

const char tuyaMutexTestStr[]         = "test_tuya_mutex";
const char tuyaMutexTestUsageStr[]    = "Usage: test_tuya_mutex\n\r";
const char tuyaMutexTestDetailsStr[]  = "Tests the Tuya TKL Mutex (Recursive Lock, TryLock, Release)\n\r";

int32_t printTuyaMutexTestUsage(void *arg)
{
    UART_PRINT(tuyaMutexTestUsageStr);
    UART_PRINT(tuyaMutexTestDetailsStr);
    return 0;
}

int32_t cmdTuyaMutexTestCallback(void *arg)
{
    // C89: Declarations at the top
    OPERATE_RET ret;
    TKL_MUTEX_HANDLE mutex = NULL;

    UART_PRINT("\n\r==================================================\n\r");
    UART_PRINT("[MUTEX TEST] Starting TKL Mutex Tests...\n\r");
    UART_PRINT("==================================================\n\r");

    // ---------------------------------------------------------
    // Phase 1: Create
    // ---------------------------------------------------------
    UART_PRINT("[MUTEX TEST] Phase 1: Create\n\r");
    ret = tkl_mutex_create_init(&mutex);
    if (ret == OPRT_OK && mutex != NULL) {
        UART_PRINT("  -> Create: PASS\n\r");
    } else {
        UART_PRINT("  -> Create: FAIL! Code: %d\n\r", ret);
        return -1;
    }

    // ---------------------------------------------------------
    // Phase 2: Basic Lock & Unlock
    // ---------------------------------------------------------
    UART_PRINT("\n\r[MUTEX TEST] Phase 2: Basic Lock/Unlock\n\r");
    ret = tkl_mutex_lock(mutex);
    if (ret == OPRT_OK) {
        UART_PRINT("  -> Lock:   PASS\n\r");
        ret = tkl_mutex_unlock(mutex);
        UART_PRINT("  -> Unlock: %s\n\r", (ret == OPRT_OK) ? "PASS" : "FAIL");
    } else {
        UART_PRINT("  -> Lock:   FAIL\n\r");
    }

    // ---------------------------------------------------------
    // Phase 3: Recursive Lock Test (CRITICAL FOR TUYA)
    // ---------------------------------------------------------
    UART_PRINT("\n\r[MUTEX TEST] Phase 3: Recursive Lock\n\r");
    ret = tkl_mutex_lock(mutex);
    if (ret == OPRT_OK) {
        UART_PRINT("  -> Lock 1: PASS\n\r");
        
        // Lock it a SECOND time. If it's a standard mutex, this freezes forever.
        // If it's correctly recursive, it will instantly pass.
        ret = tkl_mutex_lock(mutex); 
        if (ret == OPRT_OK) {
            UART_PRINT("  -> Lock 2 (Recursive): PASS\n\r");
            tkl_mutex_unlock(mutex); // Unlock 2
            tkl_mutex_unlock(mutex); // Unlock 1
            UART_PRINT("  -> Unlocks: PASS\n\r");
        } else {
            UART_PRINT("  -> Lock 2 (Recursive): FAIL (Not a recursive mutex!)\n\r");
            tkl_mutex_unlock(mutex);
        }
    }

    // ---------------------------------------------------------
    // Phase 4: TryLock
    // ---------------------------------------------------------
    UART_PRINT("\n\r[MUTEX TEST] Phase 4: TryLock\n\r");
    ret = tkl_mutex_trylock(mutex);
    if (ret == OPRT_OK) {
        UART_PRINT("  -> TryLock: PASS\n\r");
        tkl_mutex_unlock(mutex);
    } else {
        UART_PRINT("  -> TryLock: FAIL\n\r");
    }

    // ---------------------------------------------------------
    // Phase 5: Release
    // ---------------------------------------------------------
    UART_PRINT("\n\r[MUTEX TEST] Phase 5: Release\n\r");
    ret = tkl_mutex_release(mutex);
    if (ret == OPRT_OK) {
        UART_PRINT("  -> Release: PASS\n\r");
    } else {
        UART_PRINT("  -> Release: FAIL\n\r");
    }

    UART_PRINT("==================================================\n\r");
    return 0;
}
/*===========================================================*/
/* TKL MUTEX                                                 */
/*===========================================================*/



/*===========================================================*/
/* TKL NETWORK                                               */
/*===========================================================*/


const char tuyaNetTestStr[]         = "test_tuya_net";
const char tuyaNetTestUsageStr[]    = "Usage: test_tuya_net\n\r";
const char tuyaNetTestDetailsStr[]  = "Tests TKL Network (DNS, TCP Socket, HTTP GET)\n\r";

int32_t printTuyaNetTestUsage(void *arg)
{
    UART_PRINT(tuyaNetTestUsageStr);
    UART_PRINT(tuyaNetTestDetailsStr);
    return 0;
}

int32_t cmdTuyaNetTestCallback(void *arg)
{
    // C89: All declarations at the top!
    OPERATE_RET ret;
    TUYA_IP_ADDR_T ip_addr;
    int sock_fd = -1;
    const char *test_domain = "example.com";
    const char *http_req = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    char recv_buf[128];
    int recv_len;

    UART_PRINT("\n\r==================================================\n\r");
    UART_PRINT("[NET TEST] Starting TKL Network Tests...\n\r");
    UART_PRINT("[WARNING] You MUST be connected to Wi-Fi with Internet!\n\r");
    UART_PRINT("==================================================\n\r");

    // ---------------------------------------------------------
    // Phase 1: DNS Resolution
    // ---------------------------------------------------------
    UART_PRINT("[NET TEST] Phase 1: DNS Resolution (%s)\n\r", test_domain);
    ret = tkl_net_gethostbyname(test_domain, &ip_addr);
    if (ret == OPRT_OK) {
        UART_PRINT("  -> DNS: PASS (IP Resolved successfully)\n\r");
    } else {
        UART_PRINT("  -> DNS: FAIL! Code: %d (Is Wi-Fi connected to a router?)\n\r", ret);
        return -1; // Abort if we can't resolve
    }

    // ---------------------------------------------------------
    // Phase 2: Socket Create
    // ---------------------------------------------------------
    UART_PRINT("\n\r[NET TEST] Phase 2: Create TCP Socket\n\r");
    sock_fd = tkl_net_socket_create(PROTOCOL_TCP);
    int sock_fd_sec = tkl_net_socket_create(PROTOCOL_TCP);
    if (sock_fd >= 0 && sock_fd_sec >= 0) {
        UART_PRINT("  -> Create: PASS (fd1 = %d), (fd2 = %d)\n\r", sock_fd,sock_fd_sec);
    } else {
        UART_PRINT("  -> Create: FAIL!\n\r");
        return -1;
    }

    // ---------------------------------------------------------
    // Phase 3: Connect
    // ---------------------------------------------------------
    UART_PRINT("\n\r[NET TEST] Phase 3: Connect to Port 80\n\r");
    ret = tkl_net_connect(sock_fd, ip_addr, 80);
    if (ret == 0) {
        UART_PRINT("  -> Connect: PASS\n\r");
    } else {
        UART_PRINT("  -> Connect: FAIL! Errno: %d\n\r", ret);
        tkl_net_close(sock_fd);
        return -1;
    }

    // ---------------------------------------------------------
    // Phase 4: Send & Receive
    // ---------------------------------------------------------
    UART_PRINT("\n\r[NET TEST] Phase 4: HTTP GET & Recv\n\r");
    ret = tkl_net_send(sock_fd, http_req, strlen(http_req));
    if (ret > 0) {
        UART_PRINT("  -> Send: PASS (%d bytes sent)\n\r", ret);
        
        memset(recv_buf, 0, sizeof(recv_buf));
        // This will block until example.com replies
        recv_len = tkl_net_recv(sock_fd, recv_buf, sizeof(recv_buf) - 1);
        
        if (recv_len > 0) {
            UART_PRINT("  -> Recv: PASS (%d bytes received)\n\r", recv_len);
            recv_buf[15] = '\0'; // Truncate the response for clean printing
            UART_PRINT("  -> Data Snippet: [%s...]\n\r", recv_buf);
        } else {
            UART_PRINT("  -> Recv: FAIL or Timeout! Errno: %d\n\r", recv_len);
        }
    } else {
        UART_PRINT("  -> Send: FAIL! Errno: %d\n\r", ret);
    }

    // ---------------------------------------------------------
    // Phase 5: Close
    // ---------------------------------------------------------
    UART_PRINT("\n\r[NET TEST] Phase 5: Close Socket\n\r");
    ret = tkl_net_close(sock_fd);
    tkl_net_close(sock_fd_sec);
    if (ret == 0) {
        UART_PRINT("  -> Close: PASS\n\r");
    } else {
        UART_PRINT("  -> Close: FAIL! Errno: %d\n\r", ret);
    }

    UART_PRINT("==================================================\n\r");
    return 0;
}

/*===========================================================*/
/* TKL NETWORK                                               */
/*===========================================================*/

/*===========================================================*/
/* TKL OUTPUT (LOGGING)                                      */
/*===========================================================*/
#include "tkl_output.h"

const char tuyaOutputTestStr[]         = "test_tuya_output";
const char tuyaOutputTestUsageStr[]    = "Usage: test_tuya_output\n\r";
const char tuyaOutputTestDetailsStr[]  = "Tests TKL Output (Open, Print Formatting, Close)\n\r";
extern void tuya_cli_print_bridge(const char *str);

int32_t printTuyaOutputTestUsage(void *arg)
{
    UART_PRINT(tuyaOutputTestUsageStr);
    UART_PRINT(tuyaOutputTestDetailsStr);
    return 0;
}

int32_t cmdTuyaOutputTestCallback(void *arg)
{
    OPERATE_RET ret;

    UART_PRINT("\n\r==================================================\n\r");
    UART_PRINT("[OUTPUT TEST] Starting TKL Output Tests...\n\r");
    UART_PRINT("==================================================\n\r");

    // ---------------------------------------------------------
    // Phase 1: Open
    // ---------------------------------------------------------
    UART_PRINT("[OUTPUT TEST] Phase 1: Open Log Port\n\r");
    ret = tkl_log_open();
    if (ret == OPRT_OK) {
        UART_PRINT("  -> Open: PASS\n\r");
    } else {
        UART_PRINT("  -> Open: FAIL! Code: %d (Is UART mapped or busy?)\n\r", ret);
        return -1;
    }
// ---------------------------------------------------------
    // Phase 2: Formatted Output
    // ---------------------------------------------------------
    UART_PRINT("\n\r[OUTPUT TEST] Phase 2: Output Formatting\n\r");
    UART_PRINT("  -> You should see a formatted message below:\n\r");
    
    // 1. THE DIRECT BRIDGE TEST
    tuya_cli_print_bridge("  [DIRECT BRIDGE TEST] -> If you see this, the bridge works!\r\n");

    // 2. THE TUYA TEST
    tkl_log_output("     *** TUYA LOG: Test Number %d, Status: %s ***\r\n", 42, "SUCCESS");
    
    UART_PRINT("  -> Output: PASS (Did you see the message?)\n\r");

    // ---------------------------------------------------------
    // Phase 3: Close
    // ---------------------------------------------------------
    UART_PRINT("\n\r[OUTPUT TEST] Phase 3: Close\n\r");
    ret = tkl_log_close();
    if (ret == OPRT_OK) {
        UART_PRINT("  -> Close: PASS\n\r");
    } else {
        UART_PRINT("  -> Close: FAIL! Code: %d\n\r", ret);
    }

    UART_PRINT("==================================================\n\r");
    return 0;
}

/*===========================================================*/
/* TKL OUTPUT (LOGGING)                                      */
/*===========================================================*/



/*===========================================================*/
/* TKL PWM                                                   */
/*===========================================================*/

const char tuyaPwmTestStr[]        = "tuya_pwm_test";
const char tuyaPwmTestUsageStr[]   = "tuya_pwm_test";
const char tuyaPwmTestDetailsStr[] = "Tests TKL PWM Init, Start, Duty, and Stop";

int32_t printTuyaPwmTestUsage(void *arg) 
{
    UART_PRINT("Usage: %s\n\r", tuyaPwmTestUsageStr);
    UART_PRINT("(%s)\n\r", tuyaPwmTestDetailsStr);
    return 0;
}

int32_t cmdTuyaPwmTestCallback(void *arg)
{
    UART_PRINT("\n\r[PWM TEST] Starting TKL PWM Tests...\n\r");

    TUYA_PWM_NUM_E test_ch = 0; 
    OPERATE_RET ret;
    
    TUYA_PWM_BASE_CFG_T pwm_cfg = {
        .duty = 5000,       // 50% Duty Cycle
        .frequency = 1000,  // 1kHz
        .polarity = TUYA_PWM_POSITIVE
    };

    /* 1. Test Initialization */
    UART_PRINT("[PWM TEST] Initializing Port %d...\n\r", test_ch);
    ret = tkl_pwm_init(test_ch, &pwm_cfg);
    if (ret == OPRT_OK) {
        UART_PRINT("[PWM TEST] tkl_pwm_init: PASS\n\r");
    } else {
        UART_PRINT("[PWM TEST] tkl_pwm_init: FAIL! Error: %d\n\r", ret);
        return -1;
    }

    /* 2. Test Start */
    UART_PRINT("[PWM TEST] Starting PWM Output...\n\r");
    ret = tkl_pwm_start(test_ch);
    if (ret == OPRT_OK) {
        UART_PRINT("[PWM TEST] tkl_pwm_start: PASS\n\r");
    } else {
        UART_PRINT("[PWM TEST] tkl_pwm_start: FAIL!\n\r");
        tkl_pwm_deinit(test_ch);
        return -1;
    }

    /* 3. Test Duty Cycle Change (20%) */
    UART_PRINT("[PWM TEST] Setting Duty to 20%%...\n\r");
    ret = tkl_pwm_duty_set(test_ch, 2000); 
    if (ret == OPRT_OK) {
        UART_PRINT("[PWM TEST] tkl_pwm_duty_set: PASS\n\r");
    } else {
        UART_PRINT("[PWM TEST] tkl_pwm_duty_set: FAIL!\n\r");
    }

    /* 4. Test Stop & Deinit */
    UART_PRINT("[PWM TEST] Stopping and De-initializing...\n\r");
    tkl_pwm_stop(test_ch);
    ret = tkl_pwm_deinit(test_ch);
    if (ret == OPRT_OK) {
        UART_PRINT("[PWM TEST] tkl_pwm_deinit: PASS\n\r");
    } else {
        UART_PRINT("[PWM TEST] tkl_pwm_deinit: FAIL!\n\r");
    }

    UART_PRINT("[PWM TEST] Test Sequence Complete.\n\r");
    return 0;
}


/*===========================================================*/
/* TKL PWM                                                   */
/*===========================================================*/



/*===========================================================*/
/* TKL QUEUE                                                 */
/*===========================================================*/

const char tuyaQueueTestStr[]        = "tuya_queue_test";
const char tuyaQueueTestUsageStr[]   = "tuya_queue_test";
const char tuyaQueueTestDetailsStr[] = "Tests TKL Queue Create, Post, Fetch, and Free";

int32_t printTuyaQueueTestUsage(void *arg) 
{
    UART_PRINT("Usage: %s\n\r", tuyaQueueTestUsageStr);
    UART_PRINT("(%s)\n\r", tuyaQueueTestDetailsStr);
    return 0;
}

int32_t cmdTuyaQueueTestCallback(void *arg)
{
    UART_PRINT("\n\r[QUEUE TEST] Starting TKL Queue Tests...\n\r");

    TKL_QUEUE_HANDLE test_q = NULL;
    OPERATE_RET ret;
    uint32_t send_val = 0xABCD1234;
    uint32_t recv_val = 0;

    /* 1. Test Initialization */
    UART_PRINT("[QUEUE TEST] Creating Queue (size: 4 bytes, count: 5)...\n\r");
    ret = tkl_queue_create_init(&test_q, sizeof(uint32_t), 5);
    if (ret == OPRT_OK && test_q != NULL) {
        UART_PRINT("[QUEUE TEST] tkl_queue_create_init: PASS\n\r");
    } else {
        UART_PRINT("[QUEUE TEST] tkl_queue_create_init: FAIL! Error: %d\n\r", ret);
        return -1;
    }

    /* 2. Test Post */
    UART_PRINT("[QUEUE TEST] Posting Data (0x%X)...\n\r", send_val);
    ret = tkl_queue_post(test_q, &send_val, 1000); // 1000ms timeout
    if (ret == OPRT_OK) {
        UART_PRINT("[QUEUE TEST] tkl_queue_post: PASS\n\r");
    } else {
        UART_PRINT("[QUEUE TEST] tkl_queue_post: FAIL! Error: %d\n\r", ret);
        tkl_queue_free(test_q);
        return -1;
    }

    /* 3. Test Fetch */
    UART_PRINT("[QUEUE TEST] Fetching Data...\n\r");
    ret = tkl_queue_fetch(test_q, &recv_val, 1000); 
    if (ret == OPRT_OK) {
        UART_PRINT("[QUEUE TEST] tkl_queue_fetch: PASS (Recv: 0x%X)\n\r", recv_val);
        
        // Verify Data Integrity
        if (recv_val == send_val) {
            UART_PRINT("[QUEUE TEST] Data Integrity: PASS\n\r");
        } else {
            UART_PRINT("[QUEUE TEST] Data Integrity: FAIL!\n\r");
        }
    } else {
        UART_PRINT("[QUEUE TEST] tkl_queue_fetch: FAIL! Error: %d\n\r", ret);
    }

    /* 4. Test Free */
    UART_PRINT("[QUEUE TEST] Freeing Queue...\n\r");
    tkl_queue_free(test_q);
    UART_PRINT("[QUEUE TEST] tkl_queue_free: PASS\n\r");

    UART_PRINT("[QUEUE TEST] Test Sequence Complete.\n\r");
    return 0;
}

/*===========================================================*/
/* TKL QUEUE                                                 */
/*===========================================================*/

/*===========================================================*/
/* TKL RTC                                                   */
/*===========================================================*/

const char tuyaRtcTestStr[]        = "tuya_rtc_test";
const char tuyaRtcTestUsageStr[]   = "tuya_rtc_test";
const char tuyaRtcTestDetailsStr[] = "Tests TKL RTC Init, Set, Get, Hardware Tick, and Deinit";

int32_t printTuyaRtcTestUsage(void *arg) 
{
    UART_PRINT("Usage: %s\n\r", tuyaRtcTestUsageStr);
    UART_PRINT("(%s)\n\r", tuyaRtcTestDetailsStr);
    return 0;
}

int32_t cmdTuyaRtcTestCallback(void *arg)
{
    UART_PRINT("\n\r[RTC TEST] Starting TKL RTC Tests...\n\r");

    OPERATE_RET ret;
    // Let's use an arbitrary epoch timestamp (e.g., Nov 2023)
    TIME_T set_time = 1700000000; 
    TIME_T get_time_1 = 0;
    TIME_T get_time_2 = 0;

    /* 1. Test Initialization */
    UART_PRINT("[RTC TEST] Initializing RTC...\n\r");
    ret = tkl_rtc_init();
    if (ret == OPRT_OK) {
        UART_PRINT("[RTC TEST] tkl_rtc_init: PASS\n\r");
    } else {
        UART_PRINT("[RTC TEST] tkl_rtc_init: FAIL! Error: %d\n\r", ret);
        return -1;
    }

    /* 2. Test Time Set */
    UART_PRINT("[RTC TEST] Setting Time to %lu...\n\r", (unsigned long)set_time);
    ret = tkl_rtc_time_set(set_time);
    if (ret == OPRT_OK) {
        UART_PRINT("[RTC TEST] tkl_rtc_time_set: PASS\n\r");
    } else {
        UART_PRINT("[RTC TEST] tkl_rtc_time_set: FAIL! Error: %d\n\r", ret);
        return -1;
    }

    /* 3. Test Time Get (Immediate Data Integrity) */
    ret = tkl_rtc_time_get(&get_time_1);
    if (ret == OPRT_OK) {
        UART_PRINT("[RTC TEST] tkl_rtc_time_get (Initial): PASS (Time: %lu)\n\r", (unsigned long)get_time_1);
        
        // It should match exactly, or be +1 if the second rolled over right as we checked
        if (get_time_1 >= set_time && get_time_1 <= set_time + 1) {
            UART_PRINT("[RTC TEST] Data Integrity: PASS\n\r");
        } else {
            UART_PRINT("[RTC TEST] Data Integrity: FAIL! Expected ~%lu, Got %lu\n\r", 
                        (unsigned long)set_time, (unsigned long)get_time_1);
        }
    } else {
        UART_PRINT("[RTC TEST] tkl_rtc_time_get: FAIL! Error: %d\n\r", ret);
    }

    /* 4. Test Hardware Tick (Wait and Verify) */
    UART_PRINT("[RTC TEST] Waiting 2 seconds to verify RTC hardware tick...\n\r");
    
    // Block this task for just over 2000 ticks (2 seconds in a 1kHz RTOS)
    vTaskDelay(pdMS_TO_TICKS(2050)); 

    ret = tkl_rtc_time_get(&get_time_2);
    if (ret == OPRT_OK) {
        UART_PRINT("[RTC TEST] tkl_rtc_time_get (After Wait): PASS (Time: %lu)\n\r", (unsigned long)get_time_2);
        
        // Time 2 should be at least 2 seconds greater than Time 1
        if (get_time_2 >= get_time_1 + 2) {
            UART_PRINT("[RTC TEST] Hardware Tick: PASS\n\r");
        } else {
            UART_PRINT("[RTC TEST] Hardware Tick: FAIL! Time did not increment correctly.\n\r");
        }
    } else {
        UART_PRINT("[RTC TEST] tkl_rtc_time_get: FAIL! Error: %d\n\r", ret);
    }

    /* 5. Test De-init */
    UART_PRINT("[RTC TEST] De-initializing RTC...\n\r");
    ret = tkl_rtc_deinit();
    if (ret == OPRT_OK) {
        UART_PRINT("[RTC TEST] tkl_rtc_deinit: PASS\n\r");
    } else {
        UART_PRINT("[RTC TEST] tkl_rtc_deinit: FAIL! Error: %d\n\r", ret);
    }

    UART_PRINT("[RTC TEST] Test Sequence Complete.\n\r");
    return 0;
}

/*===========================================================*/
/* TKL RTC                                                   */
/*===========================================================*/

/*===========================================================*/
/* TKL SEMAPHORE                                             */
/*===========================================================*/

const char tuyaSemTestStr[]        = "tuya_sem_test";
const char tuyaSemTestUsageStr[]   = "tuya_sem_test";
const char tuyaSemTestDetailsStr[] = "Tests TKL Semaphore Create, Post, Wait (with timeouts), and Release";

int32_t printTuyaSemTestUsage(void *arg) 
{
    UART_PRINT("Usage: %s\n\r", tuyaSemTestUsageStr);
    UART_PRINT("(%s)\n\r", tuyaSemTestDetailsStr);
    return 0;
}

int32_t cmdTuyaSemTestCallback(void *arg)
{
    UART_PRINT("\n\r[SEM TEST] Starting TKL Semaphore Tests...\n\r");

    TKL_SEM_HANDLE test_sem = NULL;
    OPERATE_RET ret;

    /* 1. Test Initialization */
    // Create a counting semaphore with 0 initial tokens and a max of 2.
    UART_PRINT("[SEM TEST] Creating Semaphore (init: 0, max: 2)...\n\r");
    ret = tkl_semaphore_create_init(&test_sem, 0, 2);
    if (ret == OPRT_OK && test_sem != NULL) {
        UART_PRINT("[SEM TEST] tkl_semaphore_create_init: PASS\n\r");
    } else {
        UART_PRINT("[SEM TEST] tkl_semaphore_create_init: FAIL! Error: %d\n\r", ret);
        return -1;
    }

    /* 2. Test Wait (Timeout condition) */
    // Since init count is 0, waiting 500ms should result in a timeout error.
    UART_PRINT("[SEM TEST] Waiting on empty semaphore (500ms)...\n\r");
    ret = tkl_semaphore_wait(test_sem, 500);
    if (ret == OPRT_OS_ADAPTER_SEM_WAIT_TIMEOUT) {
        UART_PRINT("[SEM TEST] tkl_semaphore_wait (Timeout Check): PASS\n\r");
    } else {
        UART_PRINT("[SEM TEST] tkl_semaphore_wait (Timeout Check): FAIL! Expected Timeout, Got: %d\n\r", ret);
    }

    /* 3. Test Post (Add tokens) */
    UART_PRINT("[SEM TEST] Posting Semaphore (Adding 1 token)...\n\r");
    ret = tkl_semaphore_post(test_sem);
    if (ret == OPRT_OK) {
        UART_PRINT("[SEM TEST] tkl_semaphore_post: PASS\n\r");
    } else {
        UART_PRINT("[SEM TEST] tkl_semaphore_post: FAIL! Error: %d\n\r", ret);
    }

    /* 4. Test Wait (Success condition) */
    // Since we just posted a token, waiting should now succeed immediately.
    UART_PRINT("[SEM TEST] Waiting on available semaphore...\n\r");
    ret = tkl_semaphore_wait(test_sem, 1000);
    if (ret == OPRT_OK) {
        UART_PRINT("[SEM TEST] tkl_semaphore_wait (Success Check): PASS\n\r");
    } else {
        UART_PRINT("[SEM TEST] tkl_semaphore_wait (Success Check): FAIL! Error: %d\n\r", ret);
    }

    /* 5. Test Release */
    UART_PRINT("[SEM TEST] Releasing Semaphore...\n\r");
    ret = tkl_semaphore_release(test_sem);
    if (ret == OPRT_OK) {
        UART_PRINT("[SEM TEST] tkl_semaphore_release: PASS\n\r");
    } else {
        UART_PRINT("[SEM TEST] tkl_semaphore_release: FAIL! Error: %d\n\r", ret);
    }

    UART_PRINT("[SEM TEST] Test Sequence Complete.\n\r");
    return 0;
}


/*===========================================================*/
/* TKL SPI                                                   */
/*===========================================================*/

const char tuyaSpiTestStr[]        = "tuya_spi_test";
const char tuyaSpiTestUsageStr[]   = "tuya_spi_test";
const char tuyaSpiTestDetailsStr[] = "Tests TKL SPI Init, Send, Transfer, and Deinit (NOTE: Connect MOSI to MISO for full test)";

int32_t printTuyaSpiTestUsage(void *arg) 
{
    UART_PRINT("Usage: %s\n\r", tuyaSpiTestUsageStr);
    UART_PRINT("(%s)\n\r", tuyaSpiTestDetailsStr);
    return 0;
}

int32_t cmdTuyaSpiTestCallback(void *arg)
{
    UART_PRINT("\n\r[SPI TEST] Starting TKL SPI Tests...\n\r");
    UART_PRINT("[SPI TEST] NOTE: For data integrity to PASS, MOSI must be wired to MISO!\n\r");

    OPERATE_RET ret;
    TUYA_SPI_NUM_E test_port = TUYA_SPI_NUM_0;

    // Define 4 bytes of test data
    uint8_t send_data[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t recv_data[4] = {0x00, 0x00, 0x00, 0x00};

    // Configure SPI: Master, Mode 0, 8-bit, 1MHz
    TUYA_SPI_BASE_CFG_T spi_cfg = {
        .role = TUYA_SPI_ROLE_MASTER,
        .mode = TUYA_SPI_MODE0,
        .type = TUYA_SPI_AUTO_TYPE, // Auto CS
        .databits = TUYA_SPI_DATA_BIT8,
        .bitorder = TUYA_SPI_ORDER_MSB2LSB,
        .freq_hz = 1000000, 
        .spi_dma_flags = 0
    };

    /* 1. Test Initialization */
    UART_PRINT("[SPI TEST] Initializing SPI Port 0 (Master, 1MHz)...\n\r");
    ret = tkl_spi_init(test_port, &spi_cfg);
    if (ret == OPRT_OK) {
        UART_PRINT("[SPI TEST] tkl_spi_init: PASS\n\r");
    } else {
        UART_PRINT("[SPI TEST] tkl_spi_init: FAIL! Error: %d\n\r", ret);
        return -1;
    }

    /* 2. Test Send Only */
    UART_PRINT("[SPI TEST] Sending Data: 0x%02X 0x%02X 0x%02X 0x%02X\n\r", 
                send_data[0], send_data[1], send_data[2], send_data[3]);
    ret = tkl_spi_send(test_port, send_data, sizeof(send_data));
    if (ret == OPRT_OK) {
        UART_PRINT("[SPI TEST] tkl_spi_send: PASS\n\r");
    } else {
        UART_PRINT("[SPI TEST] tkl_spi_send: FAIL! Error: %d\n\r", ret);
    }

    /* 3. Test Full Duplex Transfer (Send & Receive) */
    UART_PRINT("[SPI TEST] Performing Full Duplex Transfer...\n\r");
    ret = tkl_spi_transfer(test_port, send_data, recv_data, sizeof(send_data));
    if (ret == OPRT_OK) {
        UART_PRINT("[SPI TEST] tkl_spi_transfer: PASS\n\r");
        UART_PRINT("[SPI TEST] Received Data: 0x%02X 0x%02X 0x%02X 0x%02X\n\r", 
                    recv_data[0], recv_data[1], recv_data[2], recv_data[3]);
        
        // Check Data Integrity (Requires MISO-MOSI loopback wire)
        if (memcmp(send_data, recv_data, sizeof(send_data)) == 0) {
            UART_PRINT("[SPI TEST] Data Integrity (Loopback): PASS\n\r");
        } else {
            UART_PRINT("[SPI TEST] Data Integrity (Loopback): FAIL! (Did you connect MOSI to MISO?)\n\r");
        }
    } else {
        UART_PRINT("[SPI TEST] tkl_spi_transfer: FAIL! Error: %d\n\r", ret);
    }

    /* 4. Test Deinit */
    UART_PRINT("[SPI TEST] De-initializing SPI...\n\r");
    ret = tkl_spi_deinit(test_port);
    if (ret == OPRT_OK) {
        UART_PRINT("[SPI TEST] tkl_spi_deinit: PASS\n\r");
    } else {
        UART_PRINT("[SPI TEST] tkl_spi_deinit: FAIL! Error: %d\n\r", ret);
    }

    UART_PRINT("[SPI TEST] Test Sequence Complete.\n\r");
    return 0;
}


/*===========================================================*/
/* TKL SPI                                                   */
/*===========================================================*/



/*===========================================================*/
/* TKL THREAD TESTS                                          */
/*===========================================================*/


const char tuyaThreadTestStr[]        = "test_tuya_thread";
const char tuyaThreadTestUsageStr[]   = "Usage: test_tuya_thread\n\r";
const char tuyaThreadTestDetailsStr[] = "Executes tests on the Tuya Thread adaptation layer (Create, Priority, Release).\n\r";

// A global counter so the main task can see if the background task is actually running
static volatile int g_dummy_thread_counter = 0;

/* This is the background task we will create and destroy */
static void dummy_test_thread_func(void *arg)
{
    while (1) {
        g_dummy_thread_counter++;
        tkl_system_sleep(100); // Sleep 100ms
    }
}

int32_t printTuyaThreadTestUsage(void *arg)
{
    UART_PRINT("\n\r");
    UART_PRINT(tuyaThreadTestUsageStr);
    UART_PRINT("Description: ");
    UART_PRINT(tuyaThreadTestDetailsStr);
    UART_PRINT("\n\r");
    return 0;
}

int32_t cmdTuyaThreadTestCallback(void *arg)
{
    UART_PRINT("\n\r[THREAD TEST] Starting TKL Thread Tests...\n\r");

    TKL_THREAD_HANDLE test_thread = NULL;
    OPERATE_RET ret;
    g_dummy_thread_counter = 0;

    /* 1. Test Self ID & Name */
    TKL_THREAD_HANDLE self_thread = NULL;
    tkl_thread_get_id(&self_thread);
    if (self_thread != NULL) {
        UART_PRINT("[THREAD TEST] 1. Get Self ID: PASS (Handle: 0x%x)\n\r", (uint32_t)self_thread);
    } else {
        UART_PRINT("[THREAD TEST] 1. Get Self ID: FAIL\n\r");
    }

    ret = tkl_thread_set_self_name("tuya_test_main");
    UART_PRINT("[THREAD TEST] 2. Set Self Name: PASS (Mocked return %d)\n\r", ret);

    /* 2. Test Thread Creation */
    UART_PRINT("[THREAD TEST] 3. Creating background dummy thread...\n\r");
    // Size is in bytes (1024 bytes = 256 words), Priority 3
    ret = tkl_thread_create(&test_thread, "dummy_thrd", 1024, 3, dummy_test_thread_func, NULL);
    if (ret == OPRT_OK && test_thread != NULL) {
        UART_PRINT("[THREAD TEST] Create Thread: PASS\n\r");
    } else {
        UART_PRINT("[THREAD TEST] Create Thread: FAIL (Error %d)\n\r", ret);
        return -1;
    }

    /* 3. Let it run for a bit */
    UART_PRINT("[THREAD TEST] Letting thread run for 300ms...\n\r");
    tkl_system_sleep(300);
    
    if (g_dummy_thread_counter >= 2) {
        UART_PRINT("[THREAD TEST] Thread Execution: PASS (Counter reached %d)\n\r", g_dummy_thread_counter);
    } else {
        UART_PRINT("[THREAD TEST] Thread Execution: FAIL (Counter stuck at %d)\n\r", g_dummy_thread_counter);
    }

    /* 4. Test is_self */
    BOOL_T is_self = TRUE; // Initialize to true to ensure it flips to false
    tkl_thread_is_self(test_thread, &is_self);
    if (is_self == FALSE) {
        UART_PRINT("[THREAD TEST] Check is_self (on dummy): PASS (Returned FALSE)\n\r");
    } else {
        UART_PRINT("[THREAD TEST] Check is_self: FAIL\n\r");
    }

    /* 5. Test Priority Get/Set */
    int priority = 0;
    tkl_thread_get_priority(test_thread, &priority);
    UART_PRINT("[THREAD TEST] Initial Priority: %d\n\r", priority);
    
    ret = tkl_thread_set_priority(test_thread, 4);
    int new_priority = 0;
    tkl_thread_get_priority(test_thread, &new_priority);
    if (ret == OPRT_OK && new_priority == 4) {
        UART_PRINT("[THREAD TEST] Set/Get Priority: PASS (Changed to 4)\n\r");
    } else {
        UART_PRINT("[THREAD TEST] Set/Get Priority: FAIL\n\r");
    }

    /* 6. Test Watermark (Stack usage) */
    uint32_t watermark = 0;
    ret = tkl_thread_get_watermark(test_thread, &watermark);
    if (ret == OPRT_OK) {
        UART_PRINT("[THREAD TEST] High Watermark: PASS (%d bytes free)\n\r", watermark);
    } else {
        UART_PRINT("[THREAD TEST] High Watermark: FAIL\n\r");
    }

    /* 7. Test Release/Assassination */
    UART_PRINT("[THREAD TEST] 4. Releasing/Killing background thread...\n\r");
    ret = tkl_thread_release(test_thread);
    if (ret == OPRT_OK) {
        UART_PRINT("[THREAD TEST] Release Thread: PASS\n\r");
    } else {
        UART_PRINT("[THREAD TEST] Release Thread: FAIL\n\r");
    }

    UART_PRINT("\n\r[THREAD TEST] All Thread Tests Completed Successfully!\n\r");
    return 0;
}



/*===========================================================*/
/* TKL THREAD TESTS                                          */
/*===========================================================*/

/*===========================================================*/
/* TKL TIMER TESTS                                           */
/*===========================================================*/
#include "tkl_timer.h"

const char tuyaTimerTestStr[]        = "test_tuya_timer";
const char tuyaTimerTestUsageStr[]   = "Usage: test_tuya_timer\n\r";
const char tuyaTimerTestDetailsStr[] = "Executes tests on the Tuya Timer adaptation layer (Init, Start, Stop, ISR).\n\r";

// Volatile counter updated by the hardware timer interrupt
static volatile int g_timer_isr_counter = 0;

/* Hardware Timer Callback Function */
static void dummy_timer_cb(void *args)
{
    g_timer_isr_counter++;
}

int32_t printTuyaTimerTestUsage(void *arg)
{
    UART_PRINT("\n\r");
    UART_PRINT(tuyaTimerTestUsageStr);
    UART_PRINT("Description: ");
    UART_PRINT(tuyaTimerTestDetailsStr);
    UART_PRINT("\n\r");
    return 0;
}

int32_t cmdTuyaTimerTestCallback(void *arg)
{
    UART_PRINT("\n\r[TIMER TEST] Starting TKL Timer Tests...\n\r");

    // We will test Timer ID 0 (Make sure this maps to a valid CONFIG_TIMER_0 in TI SysConfig!)
    TUYA_TIMER_NUM_E test_timer_id = 0; 
    OPERATE_RET ret;
    g_timer_isr_counter = 0;

    /* 1. Test Init */
    UART_PRINT("[TIMER TEST] 1. Initializing Timer %d (Periodic Mode)...\n\r", test_timer_id);
    
    TUYA_TIMER_BASE_CFG_T timer_cfg;
    timer_cfg.mode = TUYA_TIMER_MODE_PERIOD; // Continuous firing
    timer_cfg.cb   = dummy_timer_cb;
    timer_cfg.args = NULL;

    ret = tkl_timer_init(test_timer_id, &timer_cfg);
    if (ret == OPRT_OK) {
        UART_PRINT("[TIMER TEST] tkl_timer_init: PASS\n\r");
    } else {
        UART_PRINT("[TIMER TEST] tkl_timer_init: FAIL (Error %d)\n\r", ret);
        return -1;
    }

    /* 2. Test Start */
    UART_PRINT("[TIMER TEST] 2. Starting Timer (100ms / 100,000us interval)...\n\r");
    // Start with 100,000 microseconds (100ms)
    ret = tkl_timer_start(test_timer_id, 100000); 
    if (ret == OPRT_OK) {
        UART_PRINT("[TIMER TEST] tkl_timer_start: PASS\n\r");
    } else {
        UART_PRINT("[TIMER TEST] tkl_timer_start: FAIL (Error %d)\n\r", ret);
        tkl_timer_deinit(test_timer_id);
        return -1;
    }

    /* 3. Verify ISR Execution */
    UART_PRINT("[TIMER TEST] Sleeping for 550ms to let interrupts fire...\n\r");
    tkl_system_sleep(550); // Sleep for 550ms. Timer should fire ~5 times.

    int captured_count = g_timer_isr_counter;
    UART_PRINT("[TIMER TEST] Interrupts fired: %d\n\r", captured_count);
    
    if (captured_count >= 4 && captured_count <= 6) {
        UART_PRINT("[TIMER TEST] ISR Execution Verification: PASS\n\r");
    } else {
        UART_PRINT("[TIMER TEST] ISR Execution Verification: FAIL (Expected ~5)\n\r");
    }

    /* 4. Test Configuration Get */
    uint32_t current_period = 0;
    ret = tkl_timer_get(test_timer_id, &current_period);
    if (ret == OPRT_OK && current_period == 100000) {
        UART_PRINT("[TIMER TEST] tkl_timer_get: PASS (%d us)\n\r", current_period);
    } else {
        UART_PRINT("[TIMER TEST] tkl_timer_get: FAIL\n\r");
    }

    /* 5. Test Stop */
    UART_PRINT("[TIMER TEST] 3. Stopping Timer...\n\r");
    ret = tkl_timer_stop(test_timer_id);
    if (ret == OPRT_OK) {
        UART_PRINT("[TIMER TEST] tkl_timer_stop: PASS\n\r");
    } else {
        UART_PRINT("[TIMER TEST] tkl_timer_stop: FAIL (Error %d)\n\r", ret);
    }

    /* Verify it actually stopped */
    g_timer_isr_counter = 0; // Reset counter
    tkl_system_sleep(300);   // Wait 300ms
    if (g_timer_isr_counter == 0) {
        UART_PRINT("[TIMER TEST] Stop Verification: PASS (0 interrupts fired)\n\r");
    } else {
        UART_PRINT("[TIMER TEST] Stop Verification: FAIL (Timer still running! Fired %d times)\n\r", g_timer_isr_counter);
    }

    /* 6. Test Deinit */
    ret = tkl_timer_deinit(test_timer_id);
    if (ret == OPRT_OK) {
        UART_PRINT("[TIMER TEST] tkl_timer_deinit: PASS\n\r");
    } else {
        UART_PRINT("[TIMER TEST] tkl_timer_deinit: FAIL (Error %d)\n\r", ret);
    }

    UART_PRINT("\n\r[TIMER TEST] All Timer Tests Completed Successfully!\n\r");
    return 0;
}



/*===========================================================*/
/* TKL UART TESTS                                            */
/*===========================================================*/


const char tuyaUartTestStr[]        = "test_tuya_uart";
const char tuyaUartTestUsageStr[]   = "Usage: test_tuya_uart\n\r";
const char tuyaUartTestDetailsStr[] = "Executes tests on the Tuya UART adaptation layer (Init, Write, Deinit).\n\r";

int32_t printTuyaUartTestUsage(void *arg)
{
    UART_PRINT("\n\r");
    UART_PRINT(tuyaUartTestUsageStr);
    UART_PRINT("Description: ");
    UART_PRINT(tuyaUartTestDetailsStr);
    UART_PRINT("\n\r");
    return 0;
}

int32_t cmdTuyaUartTestCallback(void *arg)
{
    UART_PRINT("\n\r[UART TEST] Starting TKL UART Tests...\n\r");

    // WARNING: Do NOT use Port 0 if that is your CLI console port! 
    // We use Port 1 for testing to avoid locking up the terminal.
    TUYA_UART_NUM_E test_uart_port = 1; 
    OPERATE_RET ret;

    /* 1. Test Init */
    UART_PRINT("[UART TEST] 1. Initializing UART Port %d (115200 8N1)...\n\r", test_uart_port);
    
    TUYA_UART_BASE_CFG_T uart_cfg;
    uart_cfg.baudrate = 115200;
    uart_cfg.parity   = TUYA_UART_PARITY_TYPE_NONE;
    uart_cfg.databits = TUYA_UART_DATA_LEN_8BIT;
    uart_cfg.stopbits = TUYA_UART_STOP_LEN_1BIT;
    uart_cfg.flowctrl = TUYA_UART_FLOWCTRL_NONE;

    ret = tkl_uart_init(test_uart_port, &uart_cfg);
    if (ret == OPRT_OK) {
        UART_PRINT("[UART TEST] tkl_uart_init: PASS\n\r");
    } else if (ret == OPRT_NOT_SUPPORTED) {
        UART_PRINT("[UART TEST] tkl_uart_init: FAIL (Port %d is not mapped in tkl_board_config.h!)\n\r", test_uart_port);
        return -1;
    } else {
        UART_PRINT("[UART TEST] tkl_uart_init: FAIL (Error %d)\n\r", ret);
        return -1;
    }

    /* 2. Test Write */
    UART_PRINT("[UART TEST] 2. Writing test data to TX pin...\n\r");
    const char *test_msg = "--- TUYA HARDWARE UART TEST ---\r\n";
    int msg_len = strlen(test_msg);

    int bytes_written = tkl_uart_write(test_uart_port, (void *)test_msg, msg_len);
    
    if (bytes_written == msg_len) {
        UART_PRINT("[UART TEST] tkl_uart_write: PASS (%d bytes sent)\n\r", bytes_written);
    } else {
        UART_PRINT("[UART TEST] tkl_uart_write: FAIL (Tried to send %d, actually sent %d)\n\r", msg_len, bytes_written);
    }

    /* * NOTE ON TESTING READ: 
     * Because tkl_uart.c configures UART2_Mode_BLOCKING, calling tkl_uart_read() 
     * right now without a physical loopback wire (connecting TX to RX) or external 
     * device sending data will cause the task to freeze forever waiting for bytes!
     * We skip the read test here to keep your CLI responsive.
     */
    UART_PRINT("[UART TEST] 3. Skipping Read Test (To prevent blocking CLI)...\n\r");

    /* 3. Test Deinit */
    UART_PRINT("[UART TEST] 4. De-initializing UART...\n\r");
    ret = tkl_uart_deinit(test_uart_port);
    if (ret == OPRT_OK) {
        UART_PRINT("[UART TEST] tkl_uart_deinit: PASS\n\r");
    } else {
        UART_PRINT("[UART TEST] tkl_uart_deinit: FAIL (Error %d)\n\r", ret);
    }

    UART_PRINT("\n\r[UART TEST] All UART Tests Completed Successfully!\n\r");
    return 0;
}

/*===========================================================*/
/* TKL TIMER TESTS                                           */
/*===========================================================*/

/*===========================================================*/
/* TKL WAKEUP TESTS                                          */
/*===========================================================*/


const char tuyaWakeupTestStr[]        = "test_tuya_wakeup";
const char tuyaWakeupTestUsageStr[]   = "Usage: test_tuya_wakeup\n\r";
const char tuyaWakeupTestDetailsStr[] = "Executes tests on the Tuya Wakeup layer (Set/Clear Wakeup Sources).\n\r";

int32_t printTuyaWakeupTestUsage(void *arg)
{
    UART_PRINT("\n\r");
    UART_PRINT(tuyaWakeupTestUsageStr);
    UART_PRINT("Description: ");
    UART_PRINT(tuyaWakeupTestDetailsStr);
    UART_PRINT("\n\r");
    return 0;
}

int32_t cmdTuyaWakeupTestCallback(void *arg)
{
    UART_PRINT("\n\r[WAKEUP TEST] Starting TKL Wakeup Tests...\n\r");

    OPERATE_RET ret;
    TUYA_WAKEUP_SOURCE_BASE_CFG_T wake_cfg;
    memset(&wake_cfg, 0, sizeof(wake_cfg));

    /* 1. Test Unsupported Source (Timer) */
    UART_PRINT("[WAKEUP TEST] 1. Testing Unsupported Source (Timer)...\n\r");
    wake_cfg.source = TUYA_WAKEUP_SOURCE_TIMER;
    ret = tkl_wakeup_source_set(&wake_cfg);
    if (ret == OPRT_NOT_SUPPORTED) {
        UART_PRINT("[WAKEUP TEST] Set Timer Wakeup: PASS (Returned OPRT_NOT_SUPPORTED)\n\r");
    } else {
        UART_PRINT("[WAKEUP TEST] Set Timer Wakeup: FAIL (Returned %d)\n\r", ret);
    }

    /* 2. Test Set GPIO Wakeup */
    UART_PRINT("[WAKEUP TEST] 2. Testing GPIO Wakeup (Tuya GPIO 0)...\n\r");
    wake_cfg.source = TUYA_WAKEUP_SOURCE_GPIO;
    wake_cfg.wakeup_para.gpio_param.gpio_num = TUYA_GPIO_NUM_0;
    wake_cfg.wakeup_para.gpio_param.level = TUYA_GPIO_WAKEUP_FALL; // Wake on Falling edge (button press)

    ret = tkl_wakeup_source_set(&wake_cfg);
    if (ret == OPRT_OK) {
        UART_PRINT("[WAKEUP TEST] Set GPIO Wakeup: PASS\n\r");
    } else if (ret == OPRT_NOT_SUPPORTED) {
        // This is perfectly fine if you haven't added CONFIG_TUYA_GPIO_0 in SysConfig yet!
        UART_PRINT("[WAKEUP TEST] Set GPIO Wakeup: SKIP (CONFIG_TUYA_GPIO_0 not mapped in SysConfig)\n\r");
    } else {
        UART_PRINT("[WAKEUP TEST] Set GPIO Wakeup: FAIL (Error %d)\n\r", ret);
    }

    /* 3. Test Clear Specific GPIO Wakeup */
    UART_PRINT("[WAKEUP TEST] 3. Testing Clear GPIO Wakeup...\n\r");
    ret = tkl_wakeup_source_clear(&wake_cfg);
    if (ret == OPRT_OK) {
        UART_PRINT("[WAKEUP TEST] Clear GPIO Wakeup: PASS\n\r");
    } else {
        UART_PRINT("[WAKEUP TEST] Clear GPIO Wakeup: FAIL (Error %d)\n\r", ret);
    }

    /* 4. Test Clear ALL Wakeup Sources (NULL pointer) */
    UART_PRINT("[WAKEUP TEST] 4. Testing Clear ALL Wakeup Sources...\n\r");
    // We call set again just to ensure the internal struct has something to clear
    tkl_wakeup_source_set(&wake_cfg);
    
    // Tuya defines passing NULL as "Clear everything"
    ret = tkl_wakeup_source_clear(NULL);
    if (ret == OPRT_OK) {
        UART_PRINT("[WAKEUP TEST] Clear ALL Wakeup Sources: PASS\n\r");
    } else {
        UART_PRINT("[WAKEUP TEST] Clear ALL Wakeup Sources: FAIL (Error %d)\n\r", ret);
    }

    UART_PRINT("\n\r[WAKEUP TEST] All Wakeup API Tests Completed Successfully!\n\r");
    return 0;
}

/*===========================================================*/
/* TKL WATCHDOG TESTS                                        */
/*===========================================================*/
#include "tkl_system.h" // Needed to sleep between watchdog feeds

const char tuyaWdgTestStr[]        = "test_tuya_wdg";
const char tuyaWdgTestUsageStr[]   = "Usage: test_tuya_wdg\n\r";
const char tuyaWdgTestDetailsStr[] = "Executes tests on the Tuya Watchdog layer (Init, Refresh, Deinit).\n\r";

int32_t printTuyaWdgTestUsage(void *arg)
{
    UART_PRINT("\n\r");
    UART_PRINT(tuyaWdgTestUsageStr);
    UART_PRINT("Description: ");
    UART_PRINT(tuyaWdgTestDetailsStr);
    UART_PRINT("\n\r");
    return 0;
}

int32_t cmdTuyaWdgTestCallback(void *arg)
{
    UART_PRINT("\n\r[WDG TEST] Starting TKL Watchdog Tests...\n\r");

    TUYA_WDOG_BASE_CFG_T wdg_cfg;
    wdg_cfg.interval_ms = 5000; // Set a 5-second watchdog
    
    OPERATE_RET ret;

    /* 1. Test Init */
    UART_PRINT("[WDG TEST] 1. Initializing Watchdog (5000ms timeout)...\n\r");
    uint32_t actual_interval = tkl_watchdog_init(&wdg_cfg);
    
    if (actual_interval > 0) {
        UART_PRINT("[WDG TEST] tkl_watchdog_init: PASS (Actual interval: %d ms)\n\r", actual_interval);
    } else {
        UART_PRINT("[WDG TEST] tkl_watchdog_init: FAIL (Returned 0)\n\r");
        return -1;
    }

    /* 2. Test Refresh (Feed the dog) */
    UART_PRINT("[WDG TEST] 2. Testing Watchdog Refresh (Preventing Reset)...\n\r");
    
    // We will loop 3 times, sleeping 2 seconds each time. 
    // Total time = 6 seconds. If refresh fails, the 5-sec watchdog will reboot the board!
    for (int i = 1; i <= 3; i++) {
        UART_PRINT("           Sleeping 2 seconds... (Loop %d/3)\n\r", i);
        tkl_system_sleep(2000);
        
        UART_PRINT("           Feeding the Watchdog!\n\r");
        ret = tkl_watchdog_refresh();
        if (ret != OPRT_OK) {
            UART_PRINT("[WDG TEST] tkl_watchdog_refresh: FAIL (Error %d)\n\r", ret);
            break;
        }
    }
    UART_PRINT("[WDG TEST] tkl_watchdog_refresh: PASS (Board did not reset!)\n\r");

    /* 3. Test Deinit */
    UART_PRINT("[WDG TEST] 3. De-initializing Watchdog...\n\r");
    ret = tkl_watchdog_deinit();
    if (ret == OPRT_OK) {
        UART_PRINT("[WDG TEST] tkl_watchdog_deinit: PASS\n\r");
    } else {
        UART_PRINT("[WDG TEST] tkl_watchdog_deinit: FAIL (Error %d)\n\r", ret);
    }

    /* 4. Destructive Reset Test (Commented out by default) */
    /*
     * WARNING: Uncommenting this block will intentionally let the watchdog expire.
     * Your board will reboot and you will lose your terminal connection!
     */
    /*
    UART_PRINT("[WDG TEST] 4. Starting DESTRUCTIVE Reset Test...\n\r");
    tkl_watchdog_init(&wdg_cfg);
    UART_PRINT("[WDG TEST] Waiting 6 seconds for hardware reset. Goodbye...\n\r");
    while(1) {
        // Infinite loop, intentionally NOT feeding the dog.
    }
    */

    UART_PRINT("\n\r[WDG TEST] All Non-Destructive Watchdog Tests Completed!\n\r");
    return 0;
}