cmake_minimum_required(VERSION 3.10)

project(pafe)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
if(WIN32)
  set(LIBUSB_1_INCLUDE_DIR "C:/libusb-1.0.22/include/libusb-1.0")
  set(LIBUSB_1_LIBRARY "C:/libusb-1.0.22/x64/Release/lib")
endif()
find_package(libusb-1.0 REQUIRED)

add_library(pafe STATIC
    src/pasori_command.c
    src/felica_command.c
    )
set_property(TARGET pafe PROPERTY POSITION_INDEPENDENT_CODE ON)
if(WIN32)
  target_link_libraries(pafe "${LIBUSB_1_LIBRARY}/libusb-1.0.lib")
else()
  target_link_libraries(pafe usb-1.0)
endif()

ADD_DEFINITIONS(-DHAVE_CONFIG_H)

option(HAVE_LIBUSB_1 "Have libusb 1.0" ON)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_cmake.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${LIBUSB_1_INCLUDE_DIRS})
