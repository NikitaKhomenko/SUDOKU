//
// Created by nikita on 5/12/19.
//

#ifndef SUDOKU_THREAD1_MAINFUNC_H
#define SUDOKU_THREAD1_MAINFUNC_H
#include "config.h"

void fork_and_assign(pthread_t thread_rows[MAT_SIZE], pthread_t thread_cols[MAT_SIZE],
                     pthread_t thread_subMat[MAT_SIZE], GAMESTATE *shared_gameState);
void read_from_terminal_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], GAMESTATE *shared_gameState);
void read_from_file_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], char *filename, GAMESTATE *shared_gameState);
void check_sudoku(char* filePath, GAMESTATE *shared_gameState);
void print_matrix(int matrix[MAT_SIZE][MAT_SIZE]);

#endif //SUDOKU_THREAD1_MAINFUNC_H
