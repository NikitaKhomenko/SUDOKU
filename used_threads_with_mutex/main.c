#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define MAT_SIZE 9
#define NUM_THREAD 3
#define SOLUTION_SIZE (MAT_SIZE * MAT_SIZE * sizeof(int))

typedef struct data {
    int solution[MAT_SIZE][MAT_SIZE];
} DATA;


void fork_and_assign(pthread_t pthread_mat_checker[NUM_THREAD], DATA *shared_data);
void read_from_terminal_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], DATA *shared_data);
void read_from_file_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], char *filename, DATA *shared_data);
void check_sudoku(char* filePath);
int check_rows(int index, int matrix[MAT_SIZE][MAT_SIZE]);
int check_cols(int index, int matrix[MAT_SIZE][MAT_SIZE]);
int check_subMat(int index, int matrix[MAT_SIZE][MAT_SIZE]);
void print_matrix(int matrix[MAT_SIZE][MAT_SIZE]);
void decide_offset(int *y_offset, int *x_offset, int subMat);
void join_threads(pthread_t pthread_mat_checker[NUM_THREAD]);
void* ThreadEntry(void * argShared);

static int result = 1;  // check result, shared by all threads
static int done = 0;    // signals when done checking matrix, turn to 1 upon data finish or result turned to 0
static int assignment_index = 0; // concurrent data to check
pthread_mutex_t mutex_result = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_done = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_assignment_index = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* ThreadEntry(void * argShared) {
    pid_t thread_id = (pid_t) syscall(__NR_gettid);
    DATA *shared_data = (DATA *) argShared;
    int assignment_result = 1;

    while (done == 0){
        pthread_mutex_lock( &mutex_assignment_index );

        if (result == 1){
            if (assignment_index < 9){
                assignment_result = check_rows(assignment_index, shared_data->solution);

            } else if (assignment_index >= 9 && assignment_index < 18) {
                assignment_result = check_cols(assignment_index - 9, shared_data->solution);

            } else if (assignment_index >= 9 && assignment_index < 27) {
                assignment_result = check_subMat(assignment_index - 18, shared_data->solution);

            } else {
                pthread_mutex_lock( &mutex_done );
                done = 1;
                pthread_cond_signal( &cond );
                pthread_mutex_unlock( & mutex_done );
                printf( "\n[thread %d] had no data left to calculate. closing\n", thread_id);

            }
            printf( "\n[thread %d] done calculating data num %d.\n", thread_id, assignment_index-1);

        } else { // if (result == 0)
            pthread_mutex_lock( &mutex_done );
            done = 1;
            pthread_cond_signal( &cond );
            pthread_mutex_unlock( &mutex_done );
            printf( "\n[thread %d] had no data left to calculate. closing\n", thread_id);

        }

        ++assignment_index;
        pthread_mutex_unlock( &mutex_assignment_index );

        // we're going to manipulate done and use the cond, so we need the mutex
        pthread_mutex_lock( &mutex_result );
        result = assignment_result;
        pthread_mutex_unlock( &mutex_result );
    }

}


///////*** main ***///////

int main(int argc, char *argv[]) {
    int matrix[MAT_SIZE][MAT_SIZE];
    pthread_t pthread_mat_checker[NUM_THREAD];
    DATA shared_assignment;

    char* filePath; // "/home/nikita/CLionProjects/my_module/demos/wrong1"; // debugging purposes

    if(argv[1] != NULL){
        filePath = argv[1];
        read_from_file_and_write_matrix(matrix, filePath, &shared_assignment);
    } else {
        read_from_terminal_and_write_matrix(matrix, &shared_assignment);
    }

    fork_and_assign(pthread_mat_checker, &shared_assignment);

    // are the other threads still busy?
    while( done == 0 ) {
        /* block this thread until another thread signals cond. While
       blocked, the mutex is released, then re-aquired before this
       thread is woken up and the call returns. */
        pthread_cond_wait( & cond, & mutex_done );

        puts( "\n[thread main] wake - cond was signalled." );

    }

    pthread_mutex_unlock( & mutex_result );
    check_sudoku(filePath);
    join_threads(pthread_mat_checker);

    return 0;
}

void fork_and_assign(pthread_t pthread_mat_checker[NUM_THREAD], DATA *shared_data) {
    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_create(&pthread_mat_checker[i], NULL, ThreadEntry, (void *) shared_data);
    }
}

void read_from_file_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], char *filename, DATA *shared_data){
    int i, j;
    char c;

    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL)
        fprintf(stderr, "File is missing.");

    for (i = 0; i < MAT_SIZE; ++i) {
        for (j = 0; j < MAT_SIZE; ++j) {
            fscanf(file, "%d%c", &matrix[i][j], &c);
        }
    }

    print_matrix(matrix);
    memcpy(shared_data->solution, matrix, SOLUTION_SIZE);

    fclose(file);
}

void read_from_terminal_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], DATA *shared_data){
    int i, j;
    char c;

    for(i = 0; i < MAT_SIZE; ++i) {
        printf("\nEnter row number %d (num by num):\n", i+1);

        for(j = 0; j < MAT_SIZE; ++j) {
            scanf("%d%c", &matrix[i][j], &c);
        }
    }

    print_matrix(shared_data->solution);
    memcpy(shared_data->solution, matrix, SOLUTION_SIZE);
}

void check_sudoku(char *filePath) {
    if (result == 1) {
        printf("\nSudoku at file '%s' is legal\n", filePath);
    } else {
        printf("\nSudoku at file '%s' is not legal\n", filePath);
    }
}

void print_matrix(int matrix[MAT_SIZE][MAT_SIZE]) {
    printf("\nYour matrix is:\n");
    for (int i = 0; i < MAT_SIZE; i++) {
        for (int j = 0; j < MAT_SIZE; j++)
            printf("%d ", matrix[i][j]);
        printf("\n");
    }
    printf("\n");
}

void join_threads(pthread_t pthread_mat_checker[NUM_THREAD]){
    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_join (pthread_mat_checker[i], NULL);
    }
}




// The algorithm: we keep a checking list, each value in the row/col/subMat represent an indexA in the checklist.
// We move through each row/col/subMat and upgrade the indexA in the checklist accordingly.
// If we find a value not in the range of 1-9 we disqualify the sudoku.
// If after iterating through each row/col/subMat we find any indexA in the checklist that it's not 1 we disqualify the sudoku.

int check_rows(int index, int matrix[MAT_SIZE][MAT_SIZE]){
    int row = index;
    printf("\nchecking row #%i", row);

    int result = 1;
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i <MAT_SIZE; ++i) {
        if (matrix[row][i] < MAT_SIZE+1 && matrix[row][i] > 0){
            ++check_list[matrix[row][i]-1];
        } else {
            result = 0;
            break;
        }
    }
    for(int k = 0; k < MAT_SIZE; ++k){
        if (check_list[k] != 1){
            result = 0;
            break;
        }
        check_list[k] = 0;
    }
    printf("   row #%i result: %i", row, result);
    return result;
}

int check_cols(int index, int matrix[MAT_SIZE][MAT_SIZE]){
    int col = index;
    printf("\nchecking col #%i", col);

    int result = 1;
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < MAT_SIZE; ++i) {
        if (matrix[i][col] < MAT_SIZE+1 && matrix[i][col] > 0){
            ++check_list[matrix[i][col]-1];
        } else {
            result = 0;
            break;
        }
    }
    for(int k = 0; k < MAT_SIZE; ++k){
        if (check_list[k] != 1){
            result = 0;
            break;
        }
        check_list[k] = 0;
    }
    printf("   col #%i result: %i", col, result);
    return result;
}

int check_subMat(int index, int matrix[MAT_SIZE][MAT_SIZE]){
    int subMat = index;
    printf("\nchecking sub matrix #%i", subMat);

    int result = 1, y_offset, x_offset;
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    decide_offset(&y_offset, &x_offset, subMat);

    for(int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (matrix[i + y_offset][j  + x_offset] < MAT_SIZE+1 &&
                matrix[i + y_offset][j  + x_offset] > 0){
                ++check_list[matrix[i + y_offset][j  + x_offset]-1];
            } else {
                result = 0;
                break;
            }
        }
    }
    for(int k = 0; k < MAT_SIZE; ++k) {
        if (check_list[k] != 1) {
            result = 0;
            break;
        }
    }
    printf("   subMat #%i result: %i", subMat, result);
    return result;
}

void decide_offset(int *y_offset, int *x_offset, int subMat){
    if(subMat >= 0 && subMat <= 2)
        *y_offset = 0;

    if(subMat >= 3 && subMat <= 5)
        *y_offset = 3;

    if(subMat >= 6 && subMat <= 8)
        *y_offset = 6;


    if(subMat == 0 || subMat == 3 || subMat == 6)
        *x_offset = 0;

    if(subMat == 1 || subMat == 4 || subMat == 7)
        *x_offset = 3;

    if(subMat == 2 || subMat == 5 || subMat == 8)
        *x_offset = 6;
}



