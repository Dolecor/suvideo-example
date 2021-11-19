cmake_minimum_required(VERSION 3.13)
project(example C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)

add_subdirectory(split-unite-video)

add_executable(example example.c)

target_include_directories(example
    PRIVATE ${PROJECT_SOURCE_DIR}/split-unite-video/include
    )

target_link_libraries(example suvideo)

add_custom_command(TARGET example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/split-unite-video/3d_party_libs/libav/bin"
        $<TARGET_FILE_DIR:example>
    )

add_custom_command(TARGET example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${PROJECT_SOURCE_DIR}/split-unite-video/out/Debug/suvideo.dll"
        $<TARGET_FILE_DIR:example>
    )
