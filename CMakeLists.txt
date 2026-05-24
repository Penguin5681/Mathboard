cmake_minimum_required(VERSION 3.20)
project(mathboard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mathboard WIN32
    src/main.cpp
    src/canvas/Canvas.cpp
    src/canvas/StrokeCapture.cpp
    src/canvas/Smoothing.cpp
)

target_link_libraries(mathboard PRIVATE
    d2d1        
    dwrite      
    user32      
    gdi32       
)

target_include_directories(mathboard PRIVATE src)