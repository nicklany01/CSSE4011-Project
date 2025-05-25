cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(prac2)

FILE(GLOB app_sources src/*.c)

FILE(GLOB lib_sources ../../lib/*.c)

target_sources(app PRIVATE ${app_sources} ${lib_sources})

target_include_directories(app PRIVATE ../../include)