# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

cmake_minimum_required(VERSION 3.5)
project(mx-snapshot)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

include_directories(
    .
)

set (SRCS
    about.cpp
    about.h
    batchprocessing.cpp
    batchprocessing.h
    cmd.cpp
    cmd.h
    images.qrc
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    settings.cpp
    settings.h
    version.h
    work.cpp
    work.h
)


add_executable(${CMAKE_PROJECT_NAME} ${SRCS})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
