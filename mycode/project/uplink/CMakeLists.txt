cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(uplink)

target_include_directories(app PRIVATE ../../include)

target_sources(app PRIVATE src/main.c src/os_ble.c ../../lib/journal.c ../../lib/friends.c ../../lib/comms.c ../../lib/os_uart.c ../../lib/ble_uuid.c)

list(APPEND ZEPHYR_EXTRA_MODULES 
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/hal/silabs
)