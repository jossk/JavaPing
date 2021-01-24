cmake_minimum_required(VERSION 3.17)
project(java_ping C)

set(CMAKE_C_STANDARD 99)

add_executable(java_ping src/main/c/main.c src/main/c/ping.c src/main/c/ping.h)
