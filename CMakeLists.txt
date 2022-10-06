cmake_minimum_required(VERSION 3.18)

project(qt-tick CXX)

set(CMAKE_CXX_STANDARD 20)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui DBus)

add_subdirectory(src)
