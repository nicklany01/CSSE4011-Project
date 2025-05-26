cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(pet)

target_sources(app PRIVATE src/bitmaps/face_angry.c src/bitmaps/face_happy.c src/bitmaps/face_sad.c src/bitmaps/face_sleepy.c src/bitmaps/scene_beach_day.c src/bitmaps/scene_beach_night.c src/bitmaps/scene_forest_day.c src/bitmaps/scene_forest_night.c src/bitmaps/scene_meadow_day.c src/bitmaps/scene_meadow_night.c src/bitmaps/scene_shop_day.c src/bitmaps/sprite_base.c src/main.c src/comms.c src/friends.c src/journal.c src/personality.c src/scenes.c src/state.c src/os_ble.c)

FILE(GLOB lib_sources ../../lib/ble_uuid.c)

target_sources(app PRIVATE ${app_sources} ${lib_sources})

target_include_directories(app PRIVATE ../../include)
