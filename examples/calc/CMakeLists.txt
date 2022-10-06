cmake_minimum_required(VERSION 3.18)

project(example-calculator CXX)

set(CMAKE_CXX_STANDARD 20)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)

add_executable(example-calculator
    calc.cpp
)

target_link_libraries(example-calculator
    PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
)
