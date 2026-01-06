cmake_minimum_required(VERSION 3.21)
project(tuya)

set(TARGET_NAME tuya)
message(STATUS "Generating target ${TARGET_NAME}")

# --- 1. DEFINE SOURCES ---

set(${TARGET_NAME}_SOURCES
    # Tuya OS Adapter (TKL)
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_system.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_wifi.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_network.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_memory.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_mutex.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_semaphore.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_thread.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_output.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_flash.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_gpio.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/src/tkl_uart.c

    # Initialization
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/init/src/tkl_init_system.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/init/src/tkl_init_network.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/init/src/tkl_init_wifi.c

    # Utilities & JSON Core
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/utilities/crc32i.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/utilities/uni_random.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/backoffAlgorithm/source/backoff_algorithm.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libcjson/cJSON/cJSON.c

    # TAL
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_system.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_thread.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_log.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/src/tal_sleep.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_wifi/src/tal_wifi.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_network/src/tal_network.c

    # Cloud Services
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/tuya_iot.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud/mqtt_service.c
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/protocol/tuya_protocol.c
)
    set(TI_SYSCONFIG_TOOL "C:/ti/sysconfig_1.26.0/sysconfig_cli.bat")
# --- 2. DEFINE INCLUDES ---

set(${TARGET_NAME}_INCLUDES
    # 1. Compiler Toolchain Includes (MUST BE FIRST)
    "C:/ti/ccs2031/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/include/c"

    # 2. Custom and Local Configs
    ${CMAKE_CURRENT_LIST_DIR}/ti_config

    # 3. Tuya Core SDK Headers (High priority)
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/common
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/include
    
    # --- FIXED: Added missing libraries for mqtt_client_interface.h ---
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/libmqtt
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/libhttp
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/libutils
    # ------------------------------------------------------------------

    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_system/include
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_network/include
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_wifi/include

    # 4. FreeRTOS Core & Config
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/source/third_party/freertos/include"
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/source/third_party/freertos/portable/GCC/ARM_CM33_NTZ/non_secure"
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/kernel/freertos/builds/.meta/cc35xx/release/pregenerated_configuration"
    
    # 5. TI SDK Base (CRUCIAL FIX: Removed 'source/ti/posix/gcc' to avoid time.h conflict)
    # "C:/ti/simplelink_wifi_sdk_9_21_00_15/source/ti/posix/gcc"  <-- DELETED THIS LINE
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/source"

    # 6. Cloud Service Subfolders
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/netmgr
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/cloud
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/schema
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/protocol
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tuya_cloud_service/tls

    # 7. Utilities, Security & Storage
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/utilities
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/include
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/port
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libtls/mbedtls-3.1.0/include
    ${CMAKE_CURRENT_LIST_DIR}/third_party/hostap/src/openssl/include
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/tal_kv/littlefs
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/libcjson/cJSON

    # 8. TAL Subfolders & Utilities
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/tal_cli
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/tal_bluetooth
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/tal_cellular
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/tal_driver
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/tal_kv
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include/tal_security
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/common/backoffAlgorithm/source/include

    # 9. TI Adaptation Headers (TKL)
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/init/include
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/system
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/wifi
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/network
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/uart
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/adc
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/bluetooth
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/flash
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/gpio
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/i2c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/i2s
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/pwm
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/rtc
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/security
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/spi
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/timer
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl/tuyaos/tuyaos_adapter/include/utilities/include

    # 10. Explicit TI SDK Base Includes
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/source/ti/drivers/net/wifi"
    "C:/ti/simplelink_wifi_sdk_9_21_00_15/kernel/freertos/posix"
    
    # 11. Property-based Includes
    $<TARGET_PROPERTY:wifi_host_driver,INCLUDE_DIRECTORIES>
)

# --- 3. COMPILER SETTINGS ---

set(${TARGET_NAME}_LIBRARIES ${CMAKE_COMMON_LIBRARY})

set(${TARGET_NAME}_DEFINITIONS 
    -D_TUYA_TI_PLATFORM_
    -DTUYA_GW_OPERATING_SYSTEM=1
    -DDeviceFamily_CC35XX
)

set(${TARGET_NAME}_COMPILER_FLAGS -Ofast -Wno-error)

# --- 4. CREATE LIBRARY ---

add_library(${TARGET_NAME} STATIC ${${TARGET_NAME}_SOURCES})

target_compile_options(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_COMPILER_FLAGS})
target_compile_definitions(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_DEFINITIONS})
target_include_directories(${TARGET_NAME} PUBLIC ${${TARGET_NAME}_INCLUDES})
target_link_libraries(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_LIBRARIES})

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/${LIB_DEST_DIR})