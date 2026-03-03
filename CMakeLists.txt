cmake_minimum_required(VERSION 3.20)
project(OpenPDF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent)
find_package(PkgConfig REQUIRED)
pkg_check_modules(POPPLER_QT6 REQUIRED poppler-qt6)

add_executable(openpdf
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/PDFViewer.h
    src/PDFViewer.cpp
    src/PDFPageWidget.h
    src/PDFPageWidget.cpp
    src/Annotation.h
    src/Annotation.cpp
)

target_link_libraries(openpdf
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    ${POPPLER_QT6_LIBRARIES}
)

target_include_directories(openpdf PRIVATE ${POPPLER_QT6_INCLUDE_DIRS})
target_compile_options(openpdf PRIVATE ${POPPLER_QT6_CFLAGS_OTHER})