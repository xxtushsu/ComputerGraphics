cmake_minimum_required(VERSION 3.5)

project(OpenGL VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Widgets REQUIRED)
target_link_libraries(helloworld Qt5::Widgets)

set(PROJECT_SOURCES
        main1.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        mainview.cpp
        mainview.h
        model.cpp
        user_input.cpp
        resources.qrc)
add_executable(OpenGL
        ${PROJECT_SOURCES}
        )

target_link_libraries(OpenGL PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)