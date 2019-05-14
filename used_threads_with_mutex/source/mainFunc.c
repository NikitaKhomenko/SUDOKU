//
// Created by nikita on 5/14/19.
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
