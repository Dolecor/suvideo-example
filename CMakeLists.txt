cmake_minimum_required(VERSION 3.13)
project(example C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)

add_subdirectory(suvideo)

add_executable(example example.c)

target_include_directories(example
    PRIVATE ${PROJECT_SOURCE_DIR}/suvideo/include
    )

target_link_libraries(example suvideo)

add_custom_command(TARGET example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/suvideo/3d_party_libs/libav/bin"
        $<TARGET_FILE_DIR:example>
    )

add_custom_command(TARGET example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${PROJECT_SOURCE_DIR}/suvideo/out/Debug/suvideo.dll"
        $<TARGET_FILE_DIR:example>
    )
