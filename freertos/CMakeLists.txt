cmake_minimum_required(VERSION 3.11.1)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

include(cross.cmake)

#################### CONFIGURABLE SECTION ###########################

#-------------------
# Project Setup
#-------------------
enable_language(C ASM)
# Project name
project(main)
# The project version number.
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   1   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

#------------------
# Modules Setup
#------------------
#list(APPEND MODULES_USED
#    simple_module
#    )
#-------------------
# MCU Setup
#-------------------
set(MCU STM32F207xx)
set(MCU_DIR components/CMSIS/Device/ST/STM32F2xx/)
set(MCU_SPEC  cortex-m3)
set(FLOAT_SPEC "-mfloat-abi=soft -mfpu=fpv4-sp-d16")

# Dont need to change this if MCU is defined correctly
# then directly set the STARTUP_FILE and SYSTEM_FILE to the required files
string(TOLOWER startup_${MCU}.s STARTUP_FILE_NAME)

set(STARTUP_FILE ${MCU_DIR}/Source/Templates/gcc/${STARTUP_FILE_NAME})
set(SYSTEM_FILE  ${MCU_DIR}/Source/Templates/system_stm32f2xx.c)

# Set the Highspeed external clock value (HSE) in MHz
set(HSE_VAL 8000000)

# Define the linker script location
set(LINKER_SCRIPT linker.ld)

#-------------------
# HAL Setup
#-------------------
set(HAL_DIR components/STM32F2xx_HAL_Driver)
#endif

#-------------------
# RTOS Setup
#-------------------
# Path to FreeRTOS
set(RTOS_DIR components/FreeRTOS)

# Path to FreeRTOS Kernel
set(RTOS_DIR_KERNEL ${RTOS_DIR}/FreeRTOS-Kernel)
# Modify this to the path where your micrcontroller specific port is
set(RTOS_DIR_KERNEL_MCU ${RTOS_DIR_KERNEL}/portable/GCC/ARM_CM3) # For cortex-m3 microcontroller
set(RTOS_HEAP    ${RTOS_DIR_KERNEL}/portable/MemMang/heap_4.c) # Select which heap implementation to use

# Path to FreeRTOS TCP
set(RTOS_DIR_PLUS ${RTOS_DIR}/FreeRTOS-Plus/Source)
set(RTOS_DIR_TCP ${RTOS_DIR_PLUS}/FreeRTOS-Plus-TCP)
set(RTOS_PORT_LAYER_TCP ${RTOS_DIR_TCP}/portable/NetworkInterface/STM32Fxx)
set(RTOS_BUFFER_TCP ${RTOS_DIR_TCP}/portable/BufferManagement/BufferAllocation_1.c)
set(RTOS_PACK_STRUCT_TCP ${RTOS_DIR_TCP}/portable/Compiler/GCC)
set(RTOS_PHY_HANDLING_TCP ${RTOS_DIR_TCP}/portable/NetworkInterface/include)
set(RTOS_PHY_HANDLING_SRC_TCP ${RTOS_DIR_TCP}/portable/NetworkInterface/Common/phyHandling.c)

# Path to MQTT Core
set(RTOS_MQTT ${RTOS_DIR_PLUS}/Application-Protocols/coreMQTT/source)
set(BACKOFF_ALGORITHM ${RTOS_DIR_PLUS}/Utilities/backoff_algorithm/source)
set(PLAIN_TEXT ${RTOS_DIR_PLUS}/Application-Protocols/network_transport/using_plaintext)
set(MBED_TLS ${RTOS_DIR_PLUS}/Application-Protocols/network_transport/using_mbedtls)
set(MBED_TLS_LIB ${RTOS_DIR}/FreeRTOS-Plus/ThirdParty/mbedtls)
set(MBED_TLS_UTILITY ${RTOS_DIR_PLUS}/Utilities/mbedtls_freertos)
set(SOCKET_WRAPPER_TCP ${RTOS_DIR_PLUS}/Application-Protocols/network_transport/sockets_wrapper/freertos_plus_tcp)
set(LOGGER ${RTOS_DIR_PLUS}/Utilities/logging)

set(MCU_TCP STM32F2xx)

#-------------------
# CMSIS Setup
#-------------------
# Set the path to the CMSIS folder
set(CMSIS_DIR components/CMSIS)

#################### ADVANCED SECTION ###########################

#-------------------
# General Flags
#-------------------
set(OBJECT_GEN_FLAGS " \
-fno-builtin \
-Wall \
-ffunction-sections -fdata-sections \
-fomit-frame-pointer \
" CACHE INTERNAL "Common flags for C/CXX/ASM/Linker")

#-------------------
# CFLAGS
#-------------------
set(CMAKE_C_FLAGS " \
" CACHE INTERNAL "C Compiler options")

#-------------------
# ASMFLAGS for cross
#-------------------
set(CMAKE_ASM_FLAGS " \
-x assembler-with-cpp \
" CACHE INTERNAL "ASM Compiler options")

#-------------------
# LFLAGS for cross
#-------------------
set(CMAKE_EXE_LINKER_FLAGS " \
-Wl,-Map=${PROJECT_NAME}.map \
-Wl,--print-memory-usage \
-Wl,--gc-sections \
" CACHE INTERNAL "Linker options")

#------------------
# Debug Flags
#------------------
set(CMAKE_C_FLAGS_DEBUG "-Og -g -gdwarf-3 -gstrict-dwarf " CACHE INTERNAL "C Compiler options for debug build type")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g -gdwarf-3 -gstrict-dwarf " CACHE INTERNAL "C++ Compiler options for debug build type")
set(CMAKE_ASM_FLAGS_DEBUG "-Og -g -gdwarf-3 -gstrict-dwarf " CACHE INTERNAL "ASM Compiler options for debug build type")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE INTERNAL "Linker options for debug build type")

#------------------
# Release Flags
#-----------------
set(CMAKE_C_FLAGS_RELEASE "-Os -flto " CACHE INTERNAL "C Compiler options for release build type")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -flto " CACHE INTERNAL "C++ Compiler options for release build type")
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "ASM Compiler options for release build type")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto " CACHE INTERNAL "Linker options for release build type")

# Control specific options if crosscompiling

message(STATUS "[INFO] Cross compiling for ${MCU}")
message(STATUS "[INFO] Startup file used is ${STARTUP_FILE}")

#-------------------
# General Flags for cross
#-------------------
string(APPEND OBJECT_GEN_FLAGS " \
-mcpu=${MCU_SPEC} \
-mthumb \
-mthumb-interwork \
-mabi=aapcs \
${FLOAT_SPEC} \
")

#-------------------
# CFLAGS for cross
#-------------------
string(APPEND CMAKE_C_FLAGS " \
${OBJECT_GEN_FLAGS} \
")

#-------------------
# ASMFLAGS for cross
#-------------------
string(APPEND CMAKE_ASM_FLAGS " \
${OBJECT_GEN_FLAGS} \
")

#-------------------
# LFLAGS for cross
#-------------------
string(APPEND CMAKE_EXE_LINKER_FLAGS " \
${OBJECT_GEN_FLAGS} \
")

find_file(LINKER_SCRIPT_PATH
    NAMES "${LINKER_SCRIPT}"
    PATHS
    ${CMAKE_CURRENT_LIST_DIR}
    )

if(DEFINED LINKER_SCRIPT_PATH)
    message(STATUS "[INFO] Using linker file at ${LINKER_SCRIPT_PATH}")
    string(APPEND CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT_PATH} ")
    string(APPEND CMAKE_EXE_LINKER_FLAGS "-L${CMAKE_CURRENT_SOURCE_DIR} ")
else()
    message(FATAL_ERROR "[ERRR] Could not find linker script ${LINKER_SCRIPT}")
endif()


######################################################################
# @Target: ${PROJECT_NAME}.elf
# @Brief: Build the actual elf file of the project
######################################################################

#-------------------
# HAL library
#-------------------
file(GLOB HAL_SRC
    "${HAL_DIR}/Src/*.c"
    "${RTOS_PORT_LAYER_TCP}/stm32fxx_hal_eth.c"
    )
add_library(hal STATIC
    ${HAL_SRC}
    )
target_include_directories(hal
    PUBLIC
    ${HAL_DIR}/Inc
    ${MCU_DIR}/Include
    ${CMSIS_DIR}/Core/Include
    ${RTOS_PORT_LAYER_TCP}
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    )
target_compile_definitions(hal
    PUBLIC
    ${MCU}
    ${MCU_TCP}
    HSE_VALUE=${HSE_VAL}
    )

#-------------------
# FreeRTOS library
#-------------------
string(APPEND CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--undefined=vTaskSwitchContext ")
file(GLOB RTOS_SRC
    "${RTOS_DIR_KERNEL}/*.c"
    "${RTOS_DIR_KERNEL_MCU}/*.c"
    ${RTOS_HEAP}
    "${RTOS_DIR_TCP}/*.c"
    "${RTOS_PORT_LAYER_TCP}/NetworkInterface.c"
    ${RTOS_PHY_HANDLING_SRC_TCP}
    ${RTOS_BUFFER_TCP}
    )
add_library(rtos STATIC
    ${RTOS_SRC}
    )
target_include_directories(rtos
    PUBLIC
    ${RTOS_DIR_KERNEL}/include
    ${RTOS_DIR_KERNEL_MCU}
    ${RTOS_DIR_TCP}/include
    ${RTOS_PACK_STRUCT_TCP}
    ${RTOS_PHY_HANDLING_TCP}
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    )
target_link_libraries(rtos
    PUBLIC
    hal
    )
target_compile_definitions(rtos
    PUBLIC
    ${MCU_TCP}
    )

#-------------------
# Main elf
#-------------------
file(GLOB MAIN_SRC
    "src/*.c"
    "../board_config/src/*.c"
    "../rhealstone/src/*.c"
    "../jitter/src/*.c"
    "../mqtt/src/*.c"
    "${RTOS_MQTT}/*.c"
    "${BACKOFF_ALGORITHM}/*.c"
    "${PLAIN_TEXT}/*.c"
#    "${MBED_TLS_LIB}/library/*.c"
#    "${MBED_TLS_UTILITY}/*.c"
#    "${MBED_TLS}/*.c"
    "${SOCKET_WRAPPER_TCP}/*.c"
    )
set(SOURCES
    ${MAIN_SRC}
    ${STARTUP_FILE}
    ${SYSTEM_FILE}
    )
add_executable(${PROJECT_NAME}.elf
    ${SOURCES}
    )
target_include_directories(${PROJECT_NAME}.elf
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/../board_config/include
    ${PROJECT_SOURCE_DIR}/../rhealstone/include
    ${PROJECT_SOURCE_DIR}/../jitter/include
    ${PROJECT_SOURCE_DIR}/../mqtt/include
    ${PROJECT_SOURCE_DIR}/../interface/include
    ${PROJECT_SOURCE_DIR}/../include
    ${RTOS_MQTT}/include
    ${RTOS_MQTT}/interface
    ${BACKOFF_ALGORITHM}/include
    ${PLAIN_TEXT}
#    ${MBED_TLS_LIB}/include
#    ${MBED_TLS_UTILITY}
#    ${MBED_TLS}
    ${SOCKET_WRAPPER_TCP}
    ${LOGGER}
    )
target_link_libraries(${PROJECT_NAME}.elf
    PUBLIC
    rtos
    hal
    ${MODULES_USED}
    )
target_compile_definitions(${PROJECT_NAME}.elf
    PUBLIC
    ${MCU}
    HSE_VALUE=${HSE_VAL}
    )

add_custom_target(${PROJECT_NAME}.bin ALL arm-none-eabi-objcopy -Obinary "${PROJECT_NAME}.elf" "${PROJECT_NAME}.bin" DEPENDS ${PROJECT_NAME}.elf)

add_custom_target (${PROJECT_NAME}.hex ALL arm-none-eabi-objcopy -Oihex "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex" DEPENDS ${PROJECT_NAME}.elf)

# Unset all cache
unset(CMAKE_TOOLCHAIN_FILE)
unset(CMAKE_BUILD_TYPE)

