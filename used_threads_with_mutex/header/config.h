//
// Created by nikita on 5/14/19.
//

#ifndef SUDOKU_THREAD_MUTEX_CONFIG_H
#define SUDOKU_THREAD_MUTEX_CONFIG_H

#include <pthread.h>

#define MAT_SIZE 9
#define NUM_THREAD 3
#define NUM_ASSIGNMENTS (MAT_SIZE * NUM_THREAD)
#define SOLUTION_SIZE (MAT_SIZE * MAT_SIZE * sizeof(int))
#define ROW "row"
#define COL "col"
#define SUB_MATRIX "sub"

extern int result;  // check result, shared by all threads
extern int done;    // signals when done checking matrix, turn to 1 upon data finish or result turned to 0
extern int assignment_index; // concurrent data to check
extern pthread_mutex_t mutex_result;
extern pthread_mutex_t mutex_done;
extern pthread_mutex_t mutex_assignment_index;
extern pthread_cond_t cond;

typedef struct data {
    int solution[MAT_SIZE][MAT_SIZE];
    char* assignments[NUM_ASSIGNMENTS];
} DATA;

#endif //SUDOKU_THREAD_MUTEX_CONFIG_H
