#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../header/mainFunc.h"
#include "../header/threadFunc.h"
#include "../header/config.h"


void fork_and_assign(pthread_t thread_rows[MAT_SIZE], pthread_t thread_cols[MAT_SIZE],
                     pthread_t thread_subMat[MAT_SIZE], GAMESTATE *shared_gameState);
void read_from_terminal_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], GAMESTATE *shared_gameState);
void read_from_file_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], char *filename, GAMESTATE *shared_gameState);
void check_sudoku(char* filePath, GAMESTATE *shared_gameState);

int main(int argc, char *argv[]) {
    int matrix[MAT_SIZE][MAT_SIZE];
    pthread_t thread_rows[MAT_SIZE];
    pthread_t thread_cols[MAT_SIZE];
    pthread_t thread_subMat[MAT_SIZE];
    GAMESTATE shared_gameState;
    char* filePath = "/home/nikita/CLionProjects/my_module/demos/demo.txt";

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

