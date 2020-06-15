cmake_minimum_required(VERSION 3.16)
project(Pole_Position_Server C)

set(CMAKE_C_STANDARD 99)

add_executable(Pole_Position_Server main.c CarsInfo.c Util.c)

target_link_libraries(Pole_Position_Server -lmingw32 wsock32 ws2_32)