//
// Created by nikita on 4/18/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAT_SIZE 9

void check_rows(int matrix[MAT_SIZE][MAT_SIZE]);
void check_cols(int matrix[MAT_SIZE][MAT_SIZE]);
void check_subMat(int matrix[MAT_SIZE][MAT_SIZE]);

static int result = 1;


int main(int argc, char *argv[]) {
    int fdRead = (int) strtoll(argv[2], NULL, 10);
    int fdWrite = (int) strtoll(argv[3], NULL, 10);
    int matrix[MAT_SIZE][MAT_SIZE];

    for(int i = 0; i < MAT_SIZE; i++){
        for(int j = 0; j < MAT_SIZE; j++){
            read(fdRead, &matrix[i][j], sizeof(int));
        }
    }
    close(fdRead);

    if (strcmp(argv[1], "rows") == 0){
        check_rows(matrix);

    } else if (strcmp(argv[1], "cols") == 0){
        check_cols(matrix);

    } else if (strcmp(argv[1], "subMat") == 0){
        check_subMat(matrix);

    } else {
        exit(1);
    }

    write(fdWrite, &result, sizeof(int));
    close(fdWrite);

    return 0;
}

// The algorithm: we keep a checking list, each value in the row/col/subMat represent an index in the checklist.
// We move through each row/col/subMat and upgrade the index in the checklist accordingly.
// If we find a value not in the range of 1-9 we disqualify the sudoku.
// If after iterating through each row/col/subMat we find any index in the checklist that it's not 1 we disqualify the sudoku.

void check_rows(int matrix[MAT_SIZE][MAT_SIZE]){
    printf("checking rows\n");
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    for(int i = 0; i < MAT_SIZE; ++i) {
        for (int j = 0; j < MAT_SIZE; ++j) {
            if (matrix[i][j] < 10 && matrix[i][j] > 0){
                ++check_list[matrix[i][j]-1];
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

    }
}

void check_cols(int matrix[MAT_SIZE][MAT_SIZE]){
    printf("checking cols\n");
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

        for(int j = 0; j < MAT_SIZE; ++j) {
            for (int i = 0; i < MAT_SIZE; ++i) {
                if (matrix[i][j] < 10 && matrix[i][j] > 0){
                    ++check_list[matrix[i][j]-1];
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
        }

}

void check_subMat(int matrix[MAT_SIZE][MAT_SIZE]){
    printf("checking sub matrix\n");
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    int x_offset = 0;
    int y_offset = 0;
    while(y_offset <= 6){
        for(int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (matrix[i + y_offset][j  + x_offset] < 10 && matrix[i + y_offset][j  + x_offset] > 0){
                    ++check_list[matrix[i + y_offset][j  + x_offset]-1];
                } else {
                    result = 0;
                    break;
                }
            }

        }
        x_offset += 3;
        if(x_offset > 6) {
            y_offset += 3;
            x_offset = 0;
        }

        for(int k = 0; k < MAT_SIZE; ++k){
            if (check_list[k] != 1){
                result = 0;
                break;
            }
            check_list[k] = 0;
        }
    }
}


