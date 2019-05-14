#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "./header/mainFunc.h"
#include "./header/threadFunc.h"
#include "./header/config.h"

int result = 1;  // check result, shared by all threads
int done = 0;    // signals when done checking matrix, turn to 1 upon data finish or result turned to 0
int assignment_index = 0; // concurrent data to check
pthread_mutex_t mutex_result = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_done = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_assignment_index = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


///////*** main ***///////

int main(int argc, char *argv[]) {
    int matrix[MAT_SIZE][MAT_SIZE];
    pthread_t pthread_mat_checker[NUM_THREAD];
    DATA shared_data;

    char* filePath; //  ="/home/nikita/CLionProjects/my_module/demos/wrong2"; // debugging purposes

    if(argv[1] != NULL){
        filePath = argv[1];
        read_from_file_and_write_matrix(matrix, filePath, &shared_data);
    } else {
        read_from_terminal_and_write_matrix(matrix, &shared_data);
    }

    fill_assignments(&shared_data);
    fork_and_assign(pthread_mat_checker, &shared_data);

    // are the other threads still busy?
    while( done == 0 ) {
        /* block this thread until another thread signals cond. While
       blocked, the mutex is released, then re-aquired before this
       thread is woken up and the call returns. */
        pthread_cond_wait( & cond, & mutex_done );

        puts( "\n[thread main] wake - cond was signalled." );
    }

    pthread_mutex_unlock( & mutex_result );
    join_threads(pthread_mat_checker);
    check_sudoku(filePath);

    return 0;
}











