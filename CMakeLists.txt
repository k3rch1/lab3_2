cmake_minimum_required(VERSION 3.16)

project(lab3_2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/qt/6.11.0/mingw_64")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

enable_testing()

add_subdirectory(${CMAKE_SOURCE_DIR}/subs/lab5)

add_subdirectory(${CMAKE_SOURCE_DIR}/tests)

#[[add_executable(gui 
    src/main.cpp
    src/gui/main_window.cpp
    include/main_window.hpp
)

target_include_directories(gui PRIVATE include)

target_link_libraries(gui PRIVATE 
    Qt6::Widgets
    lab5
    lab6
)]]