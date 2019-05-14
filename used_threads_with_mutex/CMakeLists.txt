cmake_minimum_required(VERSION 3.10)
project(sudoku_thread_mutex C)

set(CMAKE_C_STANDARD 99)

add_executable(sudoku_thread_mutex main.c header/config.h source/mainFunc.c header/mainFunc.h source/threadFunc.c header/threadFunc.h)