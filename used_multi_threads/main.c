#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#define MAT_SIZE 9
#define SOLUTION_SIZE (MAT_SIZE * MAT_SIZE * sizeof(int))

typedef struct gameState {
    int solution[MAT_SIZE][MAT_SIZE];
    int status_rows[MAT_SIZE];
    int status_cols[MAT_SIZE];
    int status_subMat[MAT_SIZE];
} GAMESTATE;

typedef struct shared {
    GAMESTATE* gameState_to_share;
    int index;
} SHARED;


void fork_and_assign(pthread_t thread_rows[MAT_SIZE], pthread_t thread_cols[MAT_SIZE],
                     pthread_t thread_subMat[MAT_SIZE], GAMESTATE *shared_gameState);
void read_from_terminal_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], GAMESTATE *shared_gameState);
void read_from_file_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], char *filename, GAMESTATE *shared_gameState);
void check_sudoku(char* filePath, GAMESTATE *shared_gameState);
void *check_rows(void * argShared);
void *check_cols(void * argShared);
void *check_subMat(void * argShared);
void print_matrix(int matrix[MAT_SIZE][MAT_SIZE]);
void decide_offset(int *y_offset, int *x_offset, int subMat);

int main(int argc, char *argv[]) {
    int matrix[MAT_SIZE][MAT_SIZE];
    pthread_t thread_rows[MAT_SIZE];
    pthread_t thread_cols[MAT_SIZE];
    pthread_t thread_subMat[MAT_SIZE];
    GAMESTATE shared_gameState;
    char* filePath;

    if(argv[1] != NULL){
        filePath = argv[1];
        read_from_file_and_write_matrix(matrix, filePath, &shared_gameState);
    } else {
        read_from_terminal_and_write_matrix(matrix, &shared_gameState);
    }

    fork_and_assign(thread_rows, thread_cols, thread_subMat, &shared_gameState);
    check_sudoku(filePath, &shared_gameState);

    return 0;
}

void fork_and_assign(pthread_t thread_rows[MAT_SIZE], pthread_t thread_cols[MAT_SIZE],
                     pthread_t thread_subMat[MAT_SIZE], GAMESTATE *shared_gameState) {
    int i;
    for (i = 0; i < MAT_SIZE; i++) {
        SHARED gameState_to_share = {shared_gameState, i};
        pthread_create(&thread_rows[i], NULL, check_rows, (void *) &gameState_to_share);
        pthread_join (thread_rows[i], NULL);
    }

    for (i = 0; i < MAT_SIZE; i++){
        SHARED gameState_to_share = {shared_gameState, i};
        pthread_create(&thread_cols[i], NULL, check_cols, (void *) &gameState_to_share);
        pthread_join (thread_cols[i], NULL);
    }

    for (i = 0; i < MAT_SIZE; i++){
        SHARED gameState_to_share = {shared_gameState, i};
        pthread_create(&thread_subMat[i], NULL, check_subMat, (void *) &gameState_to_share);
        pthread_join (thread_subMat[i], NULL);
    }

}

void read_from_file_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], char *filename, GAMESTATE *shared_gameState){
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
    memcpy(shared_gameState->solution, matrix, SOLUTION_SIZE);

    fclose(file);
}

void read_from_terminal_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], GAMESTATE *shared_gameState){
    int i, j;
    char c;

    for(i = 0; i < MAT_SIZE; ++i) {
        printf("\nEnter row number %d (num by num):\n", i+1);

        for(j = 0; j < MAT_SIZE; ++j) {
            scanf("%d%c", &matrix[i][j], &c);
        }
    }

    print_matrix(shared_gameState->solution);
    memcpy(shared_gameState->solution, matrix, SOLUTION_SIZE);
}

void check_sudoku(char *filePath, GAMESTATE *shared_gameState) {
    int final_result = 0, i;
    for(i = 0; i < MAT_SIZE; ++i)
        final_result += shared_gameState->status_rows[i];

    for(i = 0; i < MAT_SIZE; ++i)
        final_result += shared_gameState->status_cols[i];

    for(i = 0; i < MAT_SIZE; ++i)
        final_result += shared_gameState->status_subMat[i];


    if (final_result == 27) {
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


// The algorithm: we keep a checking list, each value in the row/col/subMat represent an index in the checklist.
// We move through each row/col/subMat and upgrade the index in the checklist accordingly.
// If we find a value not in the range of 1-9 we disqualify the sudoku.
// If after iterating through each row/col/subMat we find any index in the checklist that it's not 1 we disqualify the sudoku.

void *check_rows(void * argShared){
    SHARED *shared_data = (SHARED *) argShared;
    int row = shared_data->index;
//    printf("\nchecking row #%i", row);

    int result = 1;
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i <MAT_SIZE; ++i) {
        if (shared_data->gameState_to_share->solution[row][i] < MAT_SIZE+1 &&
            shared_data->gameState_to_share->solution[row][i] > 0){
            ++check_list[shared_data->gameState_to_share->solution[row][i]-1];
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
//    printf("   row #%i result: %i", row, result);
    shared_data->gameState_to_share->status_rows[row] = result;
    pthread_exit (NULL);
}

void *check_cols(void * argShared){
    SHARED *shared_data = (SHARED *) argShared;
    int col = shared_data->index;
//    printf("\nchecking col #%i", col);

    int result = 1;
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < MAT_SIZE; ++i) {
        if (shared_data->gameState_to_share->solution[i][col] < MAT_SIZE+1 && shared_data->gameState_to_share->solution[i][col] > 0){
            ++check_list[shared_data->gameState_to_share->solution[i][col]-1];
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
//    printf("   col #%i result: %i", col, result);
    shared_data->gameState_to_share->status_cols[col] = result;
    pthread_exit (NULL);
}

void *check_subMat(void * argShared){
    SHARED *shared_data = (SHARED *) argShared;
    int subMat = shared_data->index;
//    printf("\nchecking sub matrix #%i", subMat);

    int result = 1, y_offset, x_offset;
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    decide_offset(&y_offset, &x_offset, subMat);

    for(int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (shared_data->gameState_to_share->solution[i + y_offset][j  + x_offset] < MAT_SIZE+1 &&
                shared_data->gameState_to_share->solution[i + y_offset][j  + x_offset] > 0){
                ++check_list[shared_data->gameState_to_share->solution[i + y_offset][j  + x_offset]-1];
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
//    printf("   subMat #%i result: %i", subMat, result);
    shared_data->gameState_to_share->status_subMat[subMat] = result;
    pthread_exit (NULL);
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



