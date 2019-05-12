//
// Created by nikita on 5/12/19.
//

#ifndef SUDOKU_THREAD1_THREADFUNC_H
#define SUDOKU_THREAD1_THREADFUNC_H

void *check_rows(void * argShared);
void *check_cols(void * argShared);
void *check_subMat(void * argShared);
void decide_offset(int *y_offset, int *x_offset, int subMat);

#endif //SUDOKU_THREAD1_THREADFUNC_H
