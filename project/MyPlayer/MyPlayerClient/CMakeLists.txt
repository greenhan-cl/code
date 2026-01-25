cmake_minimum_required(VERSION 3.19)
project(MyPlayerClient LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

qt_standard_project_setup()

# 确保自动处理资源文件
set(CMAKE_AUTORCC ON)
# set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/images/bitPlayer.ico)
# set(APP_RC ${CMAKE_CURRENT_SOURCE_DIR}/images/bitPlayer.rc)
set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/images/MyPlayer.ico)
set(APP_RC ${CMAKE_CURRENT_SOURCE_DIR}/images/MyPlayer.rc)
qt_add_executable(MyPlayerClient
    WIN32 MACOSX_BUNDLE
    main.cpp
    myplayer.cpp
    myplayer.h
    myplayer.ui
    login.h login.cpp Login.ui
    startuppage.h startuppage.cpp
    imageRes.qrc
    pageswitchbutton.h pageswitchbutton.cpp
    util.h
    homepagewidget.h homepagewidget.cpp homepagewidget.ui
    searchlineedit.h searchlineedit.cpp
    videobox.h videobox.cpp videobox.ui
    playerpage.h playerpage.cpp playerpage.ui
    playslider.h playslider.cpp playslider.ui
    playspeed.h playspeed.cpp playspeed.ui
    volume.h volume.cpp volume.ui
    barrageedit.h barrageedit.cpp
    myselfwidget.h myselfwidget.cpp myselfwidget.ui
    modifymyselfdialog.h modifymyselfdialog.cpp modifymyselfdialog.ui
    newpassworddialog.h newpassworddialog.cpp newpassworddialog.ui
    edituserdialog.h edituserdialog.cpp EditUserDialog.ui
    avatarbutton.h avatarbutton.cpp
    uploadvideopage.h uploadvideopage.cpp UploadVideoPage.ui
    adminwidget.h adminwidget.cpp AdminWidget.ui
    checktable.h checktable.cpp CheckTable.ui
    checktableitem.h checktableitem.cpp CheckTableItem.ui
    roletable.h roletable.cpp RoleTable.ui
    roletableitem.h roletableitem.cpp RoleTableItem.ui
    ${APP_RC}
    pagebutton.h pagebutton.cpp
    paginator.h paginator.cpp
    toast.h toast.cpp
    mpv/mpv/render_gl.h mpv/mpv/stream_cb.h mpv/client.h mpv/mpvplayer.cpp mpv/mpvplayer.h mpv/render.h mpv/render_gl.h mpv/stream_cb.h
    model/datacenter.h model/datacenter.cpp
    model/data.h model/data.cpp
)

target_link_libraries(MyPlayerClient
    PRIVATE
        Qt::Core
        Qt::Widgets
)

set(MPV_DLL ${CMAKE_CURRENT_SOURCE_DIR}/mpv/dll/libmpv-2.dll) #设置mpv库⽬录
target_link_libraries(MyPlayerClient PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
${MPV_DLL} Qt6::Network))

# 让 uic 生成的 `#include <pageswitchbutton.h>` 能找到工程目录下的头文件
target_include_directories(MyPlayerClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(GNUInstallDirs)

install(TARGETS MyPlayerClient
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET MyPlayerClient
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
