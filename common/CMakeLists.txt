cmake_minimum_required(VERSION 3.20)

project(Common LANGUAGES C CXX)

option(BUILD_SHARED_LIBS "Build Common as a shared library" OFF)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(COMMON_PLATFORM_DIRECTORY win64)
    else()
        set(COMMON_PLATFORM_DIRECTORY win32)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(COMMON_PLATFORM_DIRECTORY linux)
else()
    string(TOLOWER "${CMAKE_SYSTEM_NAME}" COMMON_PLATFORM_DIRECTORY)
endif()

if(BUILD_SHARED_LIBS)
    set(COMMON_LIBRARY_DIRECTORY shared)
else()
    set(COMMON_LIBRARY_DIRECTORY static)
endif()

set(COMMON_OUTPUT_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/${COMMON_PLATFORM_DIRECTORY}/${COMMON_LIBRARY_DIRECTORY}"
)

set(COMMON_SOURCES
    src/base64.cpp
    src/devicehelper.cpp
    src/dnssoloader.cpp
    src/fileutil.cpp
    src/inifile.cpp
    src/iputil.cpp
    src/logger.cpp
    src/md5sum.c
    src/mysock.cpp
    src/optutil.cpp
    src/pathutil.cpp
    src/processhelper.cpp
    src/quickinifile.cpp
    src/soloader.cpp
    src/strutil.cpp
    src/sysutil.cpp
    src/tcputil.cpp
    src/threadworker.cpp
    src/timeutil.cpp
)

set(COMMON_PUBLIC_HEADERS
    include/BandLimiter.h
    include/base64.h
    include/common.h
    include/common_ext.h
    include/cqueue.h
    include/cqueue_fix.h
    include/devicehelper.h
    include/dnssoloader.h
    include/dnsutil.h
    include/exputil.h
    include/fileutil.h
    include/inifile.h
    include/iputil.h
    include/logger.h
    include/macroutil.h
    include/md5sum.h
    include/mysock.h
    include/mysock_define.h
    include/mystdint.h
    include/mytimer.h
    include/optutil.h
    include/oswsharem.h
    include/pathutil.h
    include/platinc.h
    include/processhelper.h
    include/processpopen.h
    include/quickinifile.h
    include/soloader.h
    include/strutil.h
    include/sysutil.h
    include/tcputil.h
    include/threadworker.h
    include/timeoutrunner.h
    include/timeutil.h
)

add_library(Common
    ${COMMON_SOURCES}
    ${COMMON_PUBLIC_HEADERS}
)

add_library(Common::Common ALIAS Common)

if(UNIX)
    find_package(Threads REQUIRED)
endif()

target_include_directories(Common
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(Common PUBLIC cxx_std_11)

set_target_properties(Common PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIRECTORY}/$<CONFIG>"
    COMPILE_PDB_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIRECTORY}/$<CONFIG>"
    DEBUG_POSTFIX d
    LIBRARY_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIRECTORY}/$<CONFIG>"
    PDB_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIRECTORY}/$<CONFIG>"
    POSITION_INDEPENDENT_CODE ON
    RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIRECTORY}/$<CONFIG>"
)

if(WIN32)
    # processpopen.cpp uses POSIX process APIs and is only used when
    # __USE_MYPOPEN__ is enabled, so it is intentionally excluded on Windows.

    # Some legacy headers check WIN32 instead of the compiler-defined _WIN32.
    # The VS2008 projects used the Unicode character set; preserve that API mode.
    target_compile_definitions(Common
        PUBLIC WIN32
        PRIVATE
            UNICODE
            _UNICODE
            _CRT_SECURE_NO_WARNINGS
    )

    # Static-library consumers also need these Windows system libraries.
    target_link_libraries(Common PUBLIC
        iphlpapi
        ws2_32
    )

    if(BUILD_SHARED_LIBS)
        # The legacy headers do not yet contain dllexport/dllimport annotations.
        # Auto-export keeps the DLL option usable during the migration phase.
        set_target_properties(Common PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
elseif(UNIX)
    target_compile_definitions(Common PUBLIC __LINUX__)
    target_link_libraries(Common PUBLIC Threads::Threads ${CMAKE_DL_LIBS})
endif()
