cmake_minimum_required(VERSION ${TI_MIN_CMAKE_VERSION})

cmake_path(GET CMAKE_CURRENT_LIST_DIR FILENAME TARGET_NAME)

message(STATUS "Generating target            ${TARGET_NAME}")

# --- 1. DEFINE SOURCES ---

# A. Tuya SDK Sources (The Stack)
# Instead of listing 100s of files manually, we recursively find them.
# Later, you can refine this to exclude modules you don't need.
file(GLOB_RECURSE TUYA_SDK_SOURCES "${CMAKE_CURRENT_LIST_DIR}/tuya_src/*.c")

# B. Your Adapter Sources (The Glue)
# This corresponds to the 'sys_arch.c' line in the LwIP example.
# You will create these files in your ti_config folder.
set(ADAPTER_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl_system.c
    ${CMAKE_CURRENT_LIST_DIR}/ti_config/tkl_wifi.c
    # Add other TKL implementation files here as you write them
)

set(${TARGET_NAME}_SOURCES
    ${TUYA_SDK_SOURCES}
    ${ADAPTER_SOURCES}
)

# --- 2. DEFINE INCLUDES ---

set(${TARGET_NAME}_INCLUDES
    # Tuya SDK Includes
    # Note: You might need to add specific subfolders (e.g., tuya_src/components/...)
    # depending on how tuyaopen structures its headers.
    ${CMAKE_CURRENT_LIST_DIR}/tuya_src/include
    
    # Your Adapter Includes
    ${CMAKE_CURRENT_LIST_DIR}/ti_config

    # TI Drivers (WiFi is crucial)
    $<TARGET_PROPERTY:wifi_host_driver,INCLUDE_DIRECTORIES>
    # You might also need FreeRTOS includes if not inherited globally:
    # $<TARGET_PROPERTY:freertos,INCLUDE_DIRECTORIES> 
)

# --- 3. COMPILER SETTINGS ---

set(${TARGET_NAME}_LIBRARIES
    ${CMAKE_COMMON_LIBRARY}
)

# Tuya often requires specific flags. -Wno-error prevents build stoppage on warnings.
set(${TARGET_NAME}_COMPILER_FLAGS -Wno-error -Ofast)

set(${TARGET_NAME}_DEFINITIONS 
    # Add Tuya specific defines here if needed, e.g.:
    # -DENABLE_IOT_DEBUG
)

# --- 4. CREATE LIBRARY ---

add_library               (${TARGET_NAME}          ${${TARGET_NAME}_SOURCES})
target_compile_options    (${TARGET_NAME} PRIVATE ${${TARGET_NAME}_COMPILER_FLAGS})
target_compile_definitions(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_DEFINITIONS})
target_include_directories(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_INCLUDES})
target_link_libraries     (${TARGET_NAME} PRIVATE ${${TARGET_NAME}_LIBRARIES})

# Set install option
install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/${LIB_DEST_DIR})