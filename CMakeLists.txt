cmake_minimum_required(VERSION 3.16)
project(modbus_adams VERSION 0.1.0)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_STANDARD 17)

set(MY_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/../libs
        )
# find_package(Boost COMPONENTS system)

add_executable(modbus_adams 
                main.cpp
)

target_include_directories(modbus_adams PUBLIC ${MY_INCLUDE_DIR})