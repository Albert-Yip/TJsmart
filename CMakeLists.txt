cmake_minimum_required(VERSION 2.8)

project(onTX2)

set(CMAKE_BUILD_TYPE "Debug")

find_package( OpenCV REQUIRED )

include_directories(${CMAKE_CURRENT_LIST_DIR}/source/queen ${CMAKE_CURRENT_LIST_DIR}/source/cam ${CMAKE_CURRENT_LIST_DIR}/source/uart)

aux_source_directory(./source/queen queen_SRC)
aux_source_directory(./source/cam cam_SRC)
aux_source_directory(./source/uart uart_SRC)

add_executable(queen ./source/main.cpp ${queen_SRC} ${uart_SRC} ${cam_SRC} )

target_link_libraries( queen ${OpenCV_LIBS} )

