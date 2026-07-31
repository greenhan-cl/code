cmake_minimum_required(VERSION 3.16)

project(doc2md
    VERSION 0.1.0
    DESCRIPTION "Qt desktop frontend for MarkItDown"
    LANGUAGES CXX
)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Pdf PdfWidgets Svg)

option(DOC2MD_BUNDLE_PYTHON
    "Copy a prepared Python runtime into the install package"
    OFF
)
set(DOC2MD_PYTHON_RUNTIME_DIR ""
    CACHE PATH
    "Prepared Python runtime containing all MarkItDown dependencies"
)

add_subdirectory(src)
add_subdirectory(apps)
