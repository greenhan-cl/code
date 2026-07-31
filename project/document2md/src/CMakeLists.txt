set(DOC2MD_UIS
    ${PROJECT_SOURCE_DIR}/forms/main_widget.ui
)

add_library(doc2md_core STATIC
    ${PROJECT_SOURCE_DIR}/include/doc2md/application_settings.hxx
    ${PROJECT_SOURCE_DIR}/include/doc2md/conversion_runner.hxx
    ${PROJECT_SOURCE_DIR}/include/doc2md/drop_area.hxx
    ${PROJECT_SOURCE_DIR}/include/doc2md/main_widget.hxx
    ${PROJECT_SOURCE_DIR}/include/doc2md/msvc_qt_compat.hpp
    ${PROJECT_SOURCE_DIR}/include/doc2md/python_environment.hxx
    ${PROJECT_SOURCE_DIR}/include/doc2md/preview_widget.hxx
    ${PROJECT_SOURCE_DIR}/include/doc2md/settings_dialog.hxx
    application_settings.cc
    conversion_runner.cc
    drop_area.cc
    main_widget.cc
    python_environment.cc
    preview_widget.cc
    settings_dialog.cc
    ${DOC2MD_UIS}
)

add_library(doc2md::core ALIAS doc2md_core)

set_target_properties(doc2md_core
    PROPERTIES
        AUTOUIC_SEARCH_PATHS "${PROJECT_SOURCE_DIR}/forms"
)

target_include_directories(doc2md_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(doc2md_core
    PUBLIC
        Qt6::Widgets
    PRIVATE
        Qt6::Pdf
        Qt6::PdfWidgets
        Qt6::Svg
)

target_compile_features(doc2md_core PUBLIC cxx_std_17)
target_compile_definitions(doc2md_core
    PRIVATE
        DOC2MD_MARKITDOWN_SOURCE_DIR="${PROJECT_SOURCE_DIR}/3rdparty/markitdown/src"
)

if(MSVC)
    target_compile_options(doc2md_core
        PRIVATE
            /W4
            /permissive-
            /utf-8
            /FI${PROJECT_SOURCE_DIR}/include/doc2md/msvc_qt_compat.hpp
    )
else()
    target_compile_options(doc2md_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
