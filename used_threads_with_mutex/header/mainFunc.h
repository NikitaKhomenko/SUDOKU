//
// Created by nikita on 5/14/19.
//

#ifndef SUDOKU_THREAD_MUTEX_MAINFUNC_H
#define SUDOKU_THREAD_MUTEX_MAINFUNC_H

#include "config.h"

void print_matrix(int matrix[MAT_SIZE][MAT_SIZE]);
void fork_and_assign(pthread_t pthread_mat_checker[NUM_THREAD], DATA *shared_data);
void read_from_terminal_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], DATA *shared_data);
void read_from_file_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], char *filename, DATA *shared_data);
void check_sudoku(char* filePath);
void join_threads(pthread_t pthread_mat_checker[NUM_THREAD]);
void fill_assignments(DATA *shared_assignment);

#endif //SUDOKU_THREAD_MUTEX_MAINFUNC_H
