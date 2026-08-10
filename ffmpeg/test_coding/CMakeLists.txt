cmake_minimum_required(VERSION 3.21)

project(ffmpeg_test_coding LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(FFMPEG_ROOT "D:/SDK/ffmpeg" CACHE PATH "FFmpeg SDK root directory")
set(FFMPEG_HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/ffmpeg")

if(NOT EXISTS "${FFMPEG_HEADERS_DIR}/libavutil/avutil.h")
    message(FATAL_ERROR "FFmpeg headers were not found under: ${FFMPEG_HEADERS_DIR}")
endif()

if(NOT EXISTS "${FFMPEG_ROOT}/lib/avutil.lib")
    message(FATAL_ERROR "FFmpeg MSVC library was not found: ${FFMPEG_ROOT}/lib/avutil.lib")
endif()

if(NOT EXISTS "${FFMPEG_ROOT}/lib/avcodec.lib")
    message(FATAL_ERROR "FFmpeg MSVC library was not found: ${FFMPEG_ROOT}/lib/avcodec.lib")
endif()

if(NOT EXISTS "${FFMPEG_ROOT}/lib/avformat.lib")
    message(FATAL_ERROR "FFmpeg MSVC library was not found: ${FFMPEG_ROOT}/lib/avformat.lib")
endif()

if(NOT EXISTS "${FFMPEG_ROOT}/lib/avdevice.lib")
    message(FATAL_ERROR "FFmpeg MSVC library was not found: ${FFMPEG_ROOT}/lib/avdevice.lib")
endif()

add_library(ffmpeg_test_coding_lib STATIC
    src/ffmpeg_info.cc
    src/coding/codec_list_demo.cc
    src/coding/h264_encoding.cc
    src/coding/hevc_encoding.cc
    src/coding/aac_encoding.cc
    src/basic_functions/initialization_demo.cc
    src/basic_functions/demuxing_demo.cc
    src/basic_functions/decoding_demo.cc
)

add_library(ffmpeg_test_coding::lib ALIAS ffmpeg_test_coding_lib)

target_include_directories(ffmpeg_test_coding_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        "${FFMPEG_HEADERS_DIR}"
)

if(MSVC)
    target_compile_options(ffmpeg_test_coding_lib  PUBLIC /utf-8)
endif()

target_link_libraries(ffmpeg_test_coding_lib
    PRIVATE
        "${FFMPEG_ROOT}/lib/avutil.lib"
        "${FFMPEG_ROOT}/lib/avcodec.lib"
        "${FFMPEG_ROOT}/lib/avformat.lib"
        "${FFMPEG_ROOT}/lib/avdevice.lib"
)

add_executable(ffmpeg_test_coding
    src/main.cc
)

target_link_libraries(ffmpeg_test_coding
    PRIVATE
        ffmpeg_test_coding::lib
)

add_custom_command(TARGET ffmpeg_test_coding POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FFMPEG_ROOT}/bin/avutil-61.dll"
        "$<TARGET_FILE_DIR:ffmpeg_test_coding>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FFMPEG_ROOT}/bin/avcodec-63.dll"
        "$<TARGET_FILE_DIR:ffmpeg_test_coding>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FFMPEG_ROOT}/bin/avformat-63.dll"
        "$<TARGET_FILE_DIR:ffmpeg_test_coding>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FFMPEG_ROOT}/bin/avdevice-63.dll"
        "$<TARGET_FILE_DIR:ffmpeg_test_coding>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FFMPEG_ROOT}/bin/avfilter-12.dll"
        "$<TARGET_FILE_DIR:ffmpeg_test_coding>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FFMPEG_ROOT}/bin/swresample-7.dll"
        "$<TARGET_FILE_DIR:ffmpeg_test_coding>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FFMPEG_ROOT}/bin/swscale-10.dll"
        "$<TARGET_FILE_DIR:ffmpeg_test_coding>"
)
