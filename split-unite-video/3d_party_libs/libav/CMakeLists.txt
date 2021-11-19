cmake_minimum_required(VERSION 3.14)
project(libav)

add_library(libav INTERFACE IMPORTED GLOBAL)

target_include_directories(libav INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(libav INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/avcodec.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/avformat.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/avfilter.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/avdevice.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/avutil.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/postproc.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/swresample.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/swscale.lib
    )