//
// Created by nikita on 4/18/19.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAT_SIZE 9

void read_matrix_from_stdin(int matrix[MAT_SIZE][MAT_SIZE]);
void check_rows(int matrix[MAT_SIZE][MAT_SIZE]);
void check_cols(int matrix[MAT_SIZE][MAT_SIZE]);
void check_subMat(int matrix[MAT_SIZE][MAT_SIZE]);

static int sudoku = 1;


int main(int argc, char *argv[]) {
 //   int matrix[MAT_SIZE][MAT_SIZE];
//    read_matrix_from_stdin(matrix);
    int matrix[MAT_SIZE][MAT_SIZE] =
            {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
};

    if (argv[1] == "rows"){
        check_subMat(matrix);

    } else if (argv[1] == "cols"){
        check_rows(matrix);

    } else if (argv[1] == "subMat"){
        check_cols(matrix);

    } else {
        exit(1);
    }

//    close(STDIN_FILENO);
//    close(STDOUT_FILENO);
//    dup(STDOUT_FILENO);
//
//    write (STDOUT_FILENO, (const void *) sudoku, sizeof(int));

    return 0;
}

void read_matrix_from_stdin(int matrix[MAT_SIZE][MAT_SIZE]) {
    int i, j;
    int height, width;
    char buf[256];

    // Read size
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        fprintf(stderr, "Illegal input\n");
        exit(1);
    }
    if (sscanf(buf, "%d %d", &width, &height) != 2) {
        fprintf(stderr, "Illegal input\n");
        exit(1);
    }

    if (height != width != MAT_SIZE) {
        fprintf(stderr, "Illegal input\n");
        exit(1);
    }

    // Read the matrix
    for (i=0; i < MAT_SIZE; ++i)
    {
        if (fgets(buf, sizeof(buf), stdin) == NULL)
        {
            printf("Illegal input\n");
            exit(1);
        }
        int consumed = 0;
        int consumed_current;
        for (j=0; j < MAT_SIZE; ++j)
        {
            // Scan from string offset "consumed"
            if (sscanf(buf+consumed, "%d%n", &matrix[i][j], &consumed_current) != 1)
            {
                printf("Illegal input\n");
                exit(1);
            }
            // Move the offset in the string
            consumed = consumed + consumed_current;
        }
    }
}

// The algorithm: we keep a checking list, each value in the row/col/subMat represent an index in the checklist.
// We move through each row/col/subMat and upgrade the index in the checklist accordingly.
// If we find a value not in the range of 1-9 we disqualify the sudoku.
// If after iterating through each row/col/subMat we find any index in the checklist that it's not 1 we disqualify the sudoku.

void check_rows(int matrix[MAT_SIZE][MAT_SIZE]){
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    while(sudoku == 1){
        for(int i = 0; i < MAT_SIZE; ++i) {
            for (int j = 0; j < MAT_SIZE; ++j) {
                if (matrix[i][j] < 10 && matrix[i][j] > 0){
                    ++check_list[matrix[i][j]];
                } else {
                    sudoku = 0;
                }
            }
            for(int k = 0; k < MAT_SIZE; ++k){
                if (check_list[k] != 1){
                    sudoku = 0;
                }
                check_list[k] = 0;
            }
        }
    }
}

void check_cols(int matrix[MAT_SIZE][MAT_SIZE]){
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    while(sudoku == 1){
        for(int j = 0; j < MAT_SIZE; ++j) {
            for (int i = 0; i < MAT_SIZE; ++i) {
                if (matrix[i][j] < 10 && matrix[i][j] > 0){
                    ++check_list[matrix[i][j]];
                } else {
                    sudoku = 0;
                }
            }
            for(int k = 0; k < MAT_SIZE; ++k){
                if (check_list[k] != 1){
                    sudoku = 0;
                }
                check_list[k] = 0;
            }
        }
    }
}

void check_subMat(int matrix[MAT_SIZE][MAT_SIZE]){
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    int x_offset = 0;
    int y_offset = 0;
    while(sudoku == 1 && y_offset <= 6){
        for(int i = 0 + x_offset ; i < 3; ++i) {
            for (int j = 0 + y_offset ; j < 3; ++j) {
                if (matrix[i][j] < 10 && matrix[i][j] > 0){
                    ++check_list[matrix[i][j]];
                } else {
                    sudoku = 0;
                }
            }
            for(int k = 0; k < MAT_SIZE; ++k){
                if (check_list[k] != 1){
                    sudoku = 0;
                }
                check_list[k] = 0;
            }
            x_offset += 3;
            if(x_offset > 6){
                x_offset = 0;
                y_offset += 3;
            }
        }
    }
}