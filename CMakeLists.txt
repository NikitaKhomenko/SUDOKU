cmake_minimum_required(VERSION 3.10)
project(my_module C)

set(CMAKE_C_STANDARD 99)

add_executable(my_module used_execvp/main.c)