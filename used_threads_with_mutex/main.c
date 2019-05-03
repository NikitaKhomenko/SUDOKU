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
#define NUM_ASSIGNMENTS (MAT_SIZE * NUM_THREAD)
#define SOLUTION_SIZE (MAT_SIZE * MAT_SIZE * sizeof(int))
#define ROW "row"
#define COL "col"
#define SUB_MATRIX "sub"

typedef struct data {
    int solution[MAT_SIZE][MAT_SIZE];
    char* assignments[NUM_ASSIGNMENTS];
} DATA;


void fork_and_assign(pthread_t pthread_mat_checker[NUM_THREAD], DATA *shared_data);
void read_from_terminal_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], DATA *shared_data);
void read_from_file_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], char *filename, DATA *shared_data);
void check_sudoku(char* filePath);
int check_rows(int index, int matrix[MAT_SIZE][MAT_SIZE], pid_t thread_id);
int check_cols(int index, int matrix[MAT_SIZE][MAT_SIZE], pid_t thread_id);
int check_subMat(int index, int matrix[MAT_SIZE][MAT_SIZE], pid_t thread_id);
void print_matrix(int matrix[MAT_SIZE][MAT_SIZE]);
void decide_offset(int *y_offset, int *x_offset, int subMat);
void join_threads(pthread_t pthread_mat_checker[NUM_THREAD]);
void fill_assignments(DATA *shared_assignment);
void* ThreadEntry(void * argShared);

static int result = 1;  // check result, shared by all threads
static int done = 0;    // signals when done checking matrix, turn to 1 upon data finish or result turned to 0
static int assignment_index = 0; // concurrent data to check
pthread_mutex_t mutex_result = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_done = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_assignment_index = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


// Each thread picks the next assignment from the assignments list,
// after done calculating it increments the global assignment index by 1 and moving to the next.
void* ThreadEntry(void * argShared) {
    pid_t thread_id = (pid_t) syscall(__NR_gettid);
    DATA *shared_data = (DATA *) argShared;
    int assignment_result = 1, index_to_check;
    char* area_to_check =  calloc(3, 3 * sizeof(char));
    char* index_temp = calloc(1, sizeof(char));

    while (done == 0){
        if (result == 1 && assignment_index < NUM_ASSIGNMENTS ) {
            pthread_mutex_lock(&mutex_assignment_index);
            memmove(area_to_check, shared_data->assignments[assignment_index], 3 * sizeof(char));
            memmove(index_temp, shared_data->assignments[assignment_index] + 4, sizeof(char));
            index_to_check = atoi(index_temp);
            ++assignment_index;
            pthread_mutex_unlock(&mutex_assignment_index);

            if (strcmp(area_to_check, ROW) == 0) {
                assignment_result = check_rows(index_to_check, shared_data->solution, thread_id);

            } else if (strcmp(area_to_check, COL) == 0) {
                assignment_result = check_cols(index_to_check, shared_data->solution, thread_id);

            } else if (strcmp(area_to_check, SUB_MATRIX) == 0) {
                assignment_result = check_subMat(index_to_check, shared_data->solution, thread_id);

            }
            printf("\n[thread %d] done calculating assignment : %s %d\n", thread_id, area_to_check, index_to_check);

        } else {
            pthread_mutex_lock(&mutex_done);
            done = 1;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutex_done);
            printf("\n[thread %d] had no data left to calculate. closing\n", thread_id);

        }

        // we're going to manipulate done and use the cond, so we need the mutex
        if (assignment_result == 0){
            pthread_mutex_lock( &mutex_result );
            result = assignment_result;
            done = 1;
            pthread_cond_signal( &cond );
            pthread_mutex_unlock( &mutex_result );
        }
    }

}


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

void fill_assignments(DATA *shared_data){
    int i;
    char num_as_string[1];

    for (i = 0; i < NUM_ASSIGNMENTS; ++i) {
        shared_data->assignments[i] = malloc(6 * sizeof(char));
    }

    for (i  = 0; i < NUM_ASSIGNMENTS; ++i) {
        if (i < MAT_SIZE){
            strcpy(shared_data->assignments[i], ROW);

        } else if (i >= MAT_SIZE && i < MAT_SIZE * 2) {
            strcpy(shared_data->assignments[i], COL);

        } else {
            strcpy(shared_data->assignments[i], SUB_MATRIX);

        }
        sprintf(num_as_string, "%i", i%9);
        strcat(shared_data->assignments[i], " ");
        strcat(shared_data->assignments[i], num_as_string);
    }
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

int check_rows(int index, int matrix[MAT_SIZE][MAT_SIZE], pid_t thread_id){
    int row = index;
    printf("\n[thread %i] checking row #%i", thread_id, row);

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
    printf("   \n[thread %i] row #%i result: %i",thread_id,  row, result);
    return result;
}

int check_cols(int index, int matrix[MAT_SIZE][MAT_SIZE], pid_t thread_id){
    int col = index;
    printf("\n[thread %i] checking col #%i", thread_id, col);

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
    printf("   \n[thread %i] col #%i result: %i",thread_id,  col, result);
    return result;
}

int check_subMat(int index, int matrix[MAT_SIZE][MAT_SIZE], pid_t thread_id){
    int subMat = index;
    printf("\n[thread %i] checking subMat #%i", thread_id, subMat);

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
    printf("   \n[thread %i] subMat #%i result: %i",thread_id,  subMat, result);
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



