//
// Created by nikita on 5/12/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include "../header/mainFunc.h"
#include "../header/config.h"
#include "../header/threadFunc.h"

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
    for(i = 0; i < MAT_SIZE; ++i) {
        final_result += shared_gameState->status_rows[i];
        final_result += shared_gameState->status_cols[i];
        final_result += shared_gameState->status_subMat[i];
    }

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


