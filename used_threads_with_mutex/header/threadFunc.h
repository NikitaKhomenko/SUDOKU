//
// Created by nikita on 5/14/19.
//

#ifndef SUDOKU_THREAD_MUTEX_THREADFUNC_H
#define SUDOKU_THREAD_MUTEX_THREADFUNC_H

#include "../header/config.h"

int check_rows(int index, int matrix[MAT_SIZE][MAT_SIZE], pid_t thread_id);
int check_cols(int index, int matrix[MAT_SIZE][MAT_SIZE], pid_t thread_id);
int check_subMat(int index, int matrix[MAT_SIZE][MAT_SIZE], pid_t thread_id);
void decide_offset(int *y_offset, int *x_offset, int subMat);
void* ThreadEntry(void * argShared);

#endif //SUDOKU_THREAD_MUTEX_THREADFUNC_H
