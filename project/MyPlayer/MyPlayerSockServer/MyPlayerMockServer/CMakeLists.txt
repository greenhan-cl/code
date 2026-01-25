cmake_minimum_required(VERSION 3.19)
project(MyPlayerMockServer LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets HttpServer)

qt_standard_project_setup()

qt_add_executable(MyPlayerMockServer
    WIN32 MACOSX_BUNDLE
    main.cpp
    myplayersockserver.cpp
    myplayersockserver.h
    myplayersockserver.ui
)

target_link_libraries(MyPlayerMockServer
    PRIVATE
        Qt::Core
        Qt::Widgets
)

target_link_libraries(MyBitPlayerMockServer PRIVATE
                        Qt${QT_VERSION_MAJOR}::Widgets Qt6::HttpServer)

include(GNUInstallDirs)

install(TARGETS MyPlayerMockServer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET MyPlayerMockServer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
