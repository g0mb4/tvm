cmake_minimum_required(VERSION 3.5)

project(tvm LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Threads)

file(GLOB SRC
    src/*.cpp
    src/*.ui
    include/*.h
)

include_directories(include)

add_executable(tvm
    ${SRC}
)

target_link_libraries(tvm PRIVATE Qt5::Widgets ${CMAKE_THREAD_LIBS_INIT})
