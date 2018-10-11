cmake_minimum_required(VERSION 3.10)

project(pafe)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
add_library(pafe
    src/pasori_command.c
    src/felica_command.c
    )
find_package(libusb-1.0 REQUIRED)
target_link_libraries(pafe usb-1.0)

ADD_DEFINITIONS(-DHAVE_CONFIG_H)

option(HAVE_LIBUSB_1 "Have libusb 1.0" ON)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_cmake.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${LIBUSB_1_INCLUDE_DIRS})
