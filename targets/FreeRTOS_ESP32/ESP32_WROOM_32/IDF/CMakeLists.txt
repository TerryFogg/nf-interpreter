#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Fix issue in IDF 3.3.1 where cpu.h gives a compile error
file(COPY "cpu.h" DESTINATION "${ESP32_IDF_PATH}/components/soc/esp32/include/soc/")


list(APPEND COMPONENT_PATH "${ESP32_IDF_PATH}/components")
#append ESP32 IDF include paths

list(APPEND IDF_SDKCONFIG "${ESP32_IDF_PATH}/examples/get-started/blink/build/include")

list(APPEND TARGET_ESP32_IDF_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/tcpip_adapter/include")

list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/lwip/lwip/src/include/lwip")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/lwip/include/lwip/port")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/lwip/lwip/src")

list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/lwip/lwip/src/include/posix")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/lwip/include/apps")


list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/newlib/platform_include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/driver/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/driver/include/driver")

list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/esp32/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/esp32/include/rom")

list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/esp_event/include")

list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/spi_flash/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/nvs_flash/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/Log/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/Ethernet/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/wpa_supplicant/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/wpa_supplicant/port/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/freertos/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/freertos/include/freertos")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/soc/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/soc/esp32/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/heap/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/app_trace/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/bt/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/wear_levelling/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/esp_ringbuf/include")

# list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/mbedtls/port/include" "${COMPONENT_PATH}/mbedtls/mbedtls/include" )

list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/nghttp/port/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/nghttp/nghttp2/lib/includes")

# list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/openssl/include")

list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/app_update/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/vfs/include")

list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/micro-ecc/micro-ecc")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/xtensa-debug-module/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/ulp/include")

list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/ulp/include")

list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/spiffs/include")
list(APPEND TARGET_ESP32_IDF_INCLUDES "${COMPONENT_PATH}/fatfs/src")

#sdkconfig reference
list(APPEND TARGET_ESP32_IDF_INCLUDES "${ESP32_IDF_PATH}/examples/get-started/blink/build/include")


list( APPEND TARGET_ESP32_IDF_INCLUDES   "${COMPONENT_PATH}/bootloader_support/include" )
list( APPEND TARGET_ESP32_IDF_INCLUDES   "${COMPONENT_PATH}/bootloader_support/include" )

set( IDF_EXAMPLE_BASE "${ESP32_IDF_PATH}/examples/get-started/blink/build" CACHE INTERNAL "make global")
set( IDF_NANOFRAMEWORK_BASE "${ESP32_IDF_PATH}/examples/get-started/blink/build" CACHE INTERNAL "make global")

# library paths for Example project
set( PROJECT_LINK_LIBS "" )

# find_library(   LIB_APPTRACE_PATH
# 				NAMES libapp_trace.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/app_trace" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_APPTRACE_PATH})

# find_library(   LIB_APPUPDATE_PATH
# 				NAMES libapp_update.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/app_update" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_APPUPDATE_PATH})

# find_library(   LIB_AWS_IOT_PATH
# 				NAMES libaws_iot.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/aws_iot" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_AWS_IOT_PATH})

# find_library(   LIB_BOOTLOADER_SUPPORT_PATH
# 				NAMES libbootloader_support.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/bootloader_support" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_BOOTLOADER_SUPPORT_PATH})

# find_library(   LIB_BT_PATH
# 				NAMES libbt.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/bt" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_BT_PATH})

# find_library(   LIB_COAP_PATH
# 				NAMES libcoap.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/coap" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_COAP_PATH})

# find_library(   LIB_CONSOLE_PATH
# 				NAMES libconsole.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/console" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_CONSOLE_PATH})

# find_library(   LIB_CXX_PATH
# 				NAMES libcxx.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/cxx" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_CXX_PATH})

# find_library(   LIB_DRIVER_PATH
# 				NAMES libdriver.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/driver" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_DRIVER_PATH})

# find_library(   LIB_ESP32_PATH
# 				NAMES libesp32.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/esp32" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_ESP32_PATH})

# find_library(   LIB_ETHERNET_PATH
# 				NAMES libethernet.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/ethernet" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_ETHERNET_PATH})

# find_library(   LIB_EXPAT_PATH
# 				NAMES libexpat.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/expat" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_EXPAT_PATH})

# find_library(   LIB_FATFS_PATH
# 				NAMES libfatfs.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/fatfs" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_FATFS_PATH})

# find_library(   LIB_FREERTOS_PATH
# 				NAMES libfreertos.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/freertos" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_FREERTOS_PATH})

# find_library(   LIB_HEAP_PATH
# 				NAMES libheap.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/heap" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_HEAP_PATH})

# find_library(   LIB_JSMN_PATH
# 				NAMES libjsmn.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/jsmn" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_JSMN_PATH})

# find_library(   LIB_JSON_PATH
# 				NAMES libjson.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/json" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_JSON_PATH})

# find_library(   LIB_SODIUM_PATH
# 				NAMES liblibsodium.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/libsodium" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_SODIUM_PATH})

# find_library(   LIB_LOG_PATH
# 				NAMES liblog.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/log" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_LOG_PATH})

# find_library(   LIB_LWIP_PATH
# 				NAMES liblwip.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/lwip" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_LWIP_PATH})

# find_library(   LIB_MBEDTLS_PATH
# 				NAMES libmbedtls.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/mbedtls" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_MBEDTLS_PATH})

# find_library(   LIB_MDNS_PATH
# 				NAMES libmdns.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/mdns" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_MDNS_PATH})

# find_library(   LIB_MICROECC_PATH
# 				NAMES libmicro-ecc.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/micro-ecc" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_MICROECC_PATH})

# find_library(   LIB_NEWLIB_PATH
# 				NAMES libnewlib.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/newlib" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_NEWLIB_PATH})

# find_library(   LIB_NGHTTP_PATH
# 				NAMES libnghttp.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/nghttp" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_NGHTTP_PATH})

# find_library(   LIB_NVSFLASH_PATH
# 				NAMES libnvs_flash.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/nvs_flash" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_NVSFLASH_PATH})

# find_library(   LIB_OPENSSL_PATH
# 				NAMES libopenssl.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/openssl" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_OPENSSL_PATH})

# find_library(   LIB_SDMMC_PATH
# 				NAMES libsdmmc.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/sdmmc" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_SDMMC_PATH})

# find_library(   LIB_SOC_PATH
# 				NAMES libsoc.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/soc" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_SOC_PATH})

# find_library(   LIB_SPIFLASH_PATH
# 				NAMES libspi_flash.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/spi_flash" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_SPIFLASH_PATH})

# find_library(   LIB_TCPIPADAPTER_PATH
# 				NAMES libtcpip_adapter.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/tcpip_adapter" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_TCPIPADAPTER_PATH})

# find_library(   LIB_ULP_PATH
# 				NAMES libulp.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/ulp" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_ULP_PATH})

# find_library(   LIB_VFS_PATH
# 				NAMES libvfs.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/vfs" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_VFS_PATH})

# find_library(   LIB_WEAR_PATH
# 				NAMES libwear_levelling.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/wear_levelling" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_WEAR_PATH})

# find_library(   LIB_WPA_PATH
# 				NAMES libwpa_supplicant.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/wpa_supplicant" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_WPA_PATH})

# find_library(   LIB_PTHREAD_PATH
# 				NAMES libpthread.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/pthread" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_PTHREAD_PATH})


# find_library(   LIB_DEBUG_PATH
# 				NAMES libxtensa-debug-module.a
# 				HINTS "${IDF_NANOFRAMEWORK_BASE}/xtensa-debug-module" 
# 				NO_CMAKE_FIND_ROOT_PATH )
# list(APPEND PROJECT_LINK_LIBS ${LIB_DEBUG_PATH})


# Path to pre compiled Libs

set( DIRECT_LINK_LIBS "" )
set( DIRECT_LINK_PATH "${ESP32_LIBS_PATH}" )
#message( "direct link path:${DIRECT_LINK_PATH}")

list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libapp_trace.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libapp_update.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libasio.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libaws_iot.a)
#list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libbootloader_support.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libbt.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libcoap.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libconsole.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libcxx.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libdriver.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libefuse.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libesp-tls.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libesp32.a)

list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libesp_adc_cal.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libesp_event.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libesp_http_client.a)

list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libesp_http_server.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libesp_https_ota.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libesp_https_server.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libesp_ringbuf.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libespcoredump.a)

list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libethernet.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libexpat.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libfatfs.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libfreemodbus.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libfreertos.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libheap.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libidf_test.a )
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libjsmn.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libjson.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/liblibsodium.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/liblog.a)
# list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libmbedtls.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libmdns.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libmicro-ecc.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libmqtt.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libnewlib.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libnghttp.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libnvs_flash.a)
# list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libopenssl.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libprotobuf-c.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libprotocomm.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libpthread.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libsdmmc.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libsmartconfig_ack.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libsoc.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libspi_flash.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libspiffs.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libtcp_transport.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libtcpip_adapter.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libulp.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libunity.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libvfs.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libwear_levelling.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libwifi_provisioning.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libwpa_supplicant.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libxtensa-debug-module.a)
list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/libbootloader_support.a)

# Lwip lib is built separately 
#list(APPEND DIRECT_LINK_LIBS ${DIRECT_LINK_PATH}/liblwip.a)

set(PROJECT_LINK_LIBS ${DIRECT_LINK_LIBS})

set(TARGET_ESP32_IDF_INCLUDES ${TARGET_ESP32_IDF_INCLUDES} CACHE INTERNAL "make global")
set(PROJECT_LINK_LIBS ${PROJECT_LINK_LIBS} CACHE INTERNAL "make global")
#set(DIRECT_LINK_LIBS ${DIRECT_LINK_LIBS} CACHE INTERNAL "make global")
