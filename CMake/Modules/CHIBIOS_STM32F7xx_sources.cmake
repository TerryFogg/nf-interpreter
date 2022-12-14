#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# from startup_stm32f7xx.mk
# List of the ChibiOS generic STM32F7xx startup and CMSIS files.

################################################################################################
# WHEN ADDING A NEW BOARD add the source code file for the hal/ports/STM32/STM32F7xx/platform.mk
################################################################################################

set(CHIBIOS_PORT_SRCS
    # startup code
    crt1.c
    vectors.S
    crt0_v7m.S

    nvic.c
    stm32_isr.c
    hal_lld.c

    hal_adc_lld.c
    hal_can_lld.c
    hal_crypto_lld.c
    hal_dac_lld.c
    stm32_dma.c
    stm32_exti.c
    hal_pal_lld.c
    hal_i2c_lld.c
    hal_mac_lld.c
    hal_usb_lld.c
    hal_wspi_lld.c 
    hal_trng_lld.c
    hal_rtc_lld.c
    hal_sdc_lld.c

    hal_i2s_lld.c
    hal_spi_lld.c

    hal_st_lld.c
    hal_gpt_lld.c
    hal_icu_lld.c
    hal_pwm_lld.c

    hal_serial_lld.c
    hal_uart_lld.c

    hal_wdg_lld.c

    # RT
    chcore.c
    chcore_v7m.c
    chcoreasm_v7m.S
)

foreach(SRC_FILE ${CHIBIOS_PORT_SRCS})
    set(CHIBIOS_F7_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(CHIBIOS_F7_SRC_FILE ${SRC_FILE}
        PATHS 

            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx/compilers/GCC
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/common/startup/ARMCMx/compilers/GCC
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/common/ARMCMx

            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/STM32F7xx

            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/ADCv2
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/CANv1
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/CRYPv1
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/DACv1
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/DMAv2
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/EXTIv1
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/GPIOv2
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/I2Cv2
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/MACv1
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/OTGv1
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/QUADSPIv1
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/RNGv1
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/RTCv2
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/SDMMCv1
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/SPIv2
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/TIMv1
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/USARTv2
            ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/xWDGv1

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${CHIBIOS_F7_SRC_FILE}") # debug helper
    endif()
    list(APPEND CHIBIOS_SOURCES ${CHIBIOS_F7_SRC_FILE})
endforeach()

list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/common/portability/GCC)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/common/startup/ARMCMx/compilers/GCC)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/common/startup/ARMCMx/devices/STM32F7xx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/common/ext/ARM/CMSIS/Core/Include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/common/ext/ST/STM32F7xx)

list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/ADCv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/CANv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/CRYPv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/DACv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/DMAv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/EXTIv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/GPIOv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/I2Cv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/MACv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/OTGv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/QUADSPIv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/RNGv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/RTCv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/SDMMCv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/SPIv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/TIMv1)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/USARTv2)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/LLD/xWDGv1)


####################################################################################
# WHEN ADDING A NEW CHIBIOS OVERLAY component add the include directory(ies) below 
####################################################################################
# component STM32_FLASH
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/FLASHv2)
# component STM32_CRC
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/CRCv1)
# component STM32_RNG
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/RNGv1) 
# component STM32_FSMC (Flexible Memory Controller)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/FSMCv1)
# component STM32_ONEWIRE
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/ONEWIREv1) 
# component STM32_QSPI (Quad-SPI)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/QSPIv1)
list(APPEND CHIBIOS_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/STM32_CubePackage_Source/Drivers/STM32F7xx_HAL_Driver/Inc")

###############################################################################################################################
# Add above the required include directory(ies) for a new nanoFramework overlay component that you are adding
# following the template below. 
#
# list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/<path-here>)
###############################################################################################################################


####################################################################################################
# WHEN ADDING A NEW CHIBIOS OVERLAY component add the source file(s) specific to this series below 
####################################################################################################
# component STM32_FLASH
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/FLASHv2/flash_lld.c)
# component STM32_CRC
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/CRCv1/crc_lld.c)
# component STM32_RNG
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/RNGv1/rng_lld.c)
# component STM32_FSMC (Flexible Memory Controller)
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/FSMCv1/fsmc_nand_lld.c)
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/FSMCv1/fsmc_sdram_lld.c)
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/FSMCv1/fsmc_sram_lld.c)
# component STM32_ONEWIRE
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/ONEWIREv1/onewire_lld.c)
# component STM32_QSPI (Quad-SPI)
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/STM32/LLD/QSPIv1/qspi_lld.c)

##########################################################################################################################
# Add above ALL the source code file(s) low level driver specif for a series required for a new nanoFramework 
# overlay component that you are adding following the template below. 
#
# list(APPEND CHIBIOS_SOURCES ${CMAKE_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/<path-here>)
##########################################################################################################################
