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

void check_rows(int matrix[MAT_SIZE][MAT_SIZE]);
void check_cols(int matrix[MAT_SIZE][MAT_SIZE]);
void check_subMat(int matrix[MAT_SIZE][MAT_SIZE]);

static int result = 1;


int main(int argc, char *argv[]) {
    int fdRead = atoi(argv[2]);
    int fdWrite = atoi(argv[3]);
    int matrix[MAT_SIZE][MAT_SIZE];
    int result = 0;

    read(fdRead, &matrix[MAT_SIZE][MAT_SIZE], MAT_SIZE * MAT_SIZE * sizeof(int));
    close(fdRead);

    if (argv[1] == "rows"){
        check_subMat(matrix);

    } else if (argv[1] == "cols"){
        check_rows(matrix);

    } else if (argv[1] == "subMat"){
        check_cols(matrix);

    } else {
        exit(1);
    }

//    result = 1;
    write(fdWrite, &result, sizeof(int));
    close(fdWrite);

    return 0;
}

// The algorithm: we keep a checking list, each value in the row/col/subMat represent an index in the checklist.
// We move through each row/col/subMat and upgrade the index in the checklist accordingly.
// If we find a value not in the range of 1-9 we disqualify the result.
// If after iterating through each row/col/subMat we find any index in the checklist that it's not 1 we disqualify the result.

void check_rows(int matrix[MAT_SIZE][MAT_SIZE]){
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    while(result == 1){
        for(int i = 0; i < MAT_SIZE; ++i) {
            for (int j = 0; j < MAT_SIZE; ++j) {
                if (matrix[i][j] < 10 && matrix[i][j] > 0){
                    ++check_list[matrix[i][j]];
                } else {
                    result = 0;
                }
            }
            for(int k = 0; k < MAT_SIZE; ++k){
                if (check_list[k] != 1){
                    result = 0;
                }
                check_list[k] = 0;
            }
        }
    }
}

void check_cols(int matrix[MAT_SIZE][MAT_SIZE]){
    int check_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    while(result == 1){
        for(int j = 0; j < MAT_SIZE; ++j) {
            for (int i = 0; i < MAT_SIZE; ++i) {
                if (matrix[i][j] < 10 && matrix[i][j] > 0){
                    ++check_list[matrix[i][j]];
                } else {
                    result = 0;
                }
            }
            for(int k = 0; k < MAT_SIZE; ++k){
                if (check_list[k] != 1){
                    result = 0;
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
    while(result == 1 && y_offset <= 6){
        for(int i = 0 + x_offset ; i < 3; ++i) {
            for (int j = 0 + y_offset ; j < 3; ++j) {
                if (matrix[i][j] < 10 && matrix[i][j] > 0){
                    ++check_list[matrix[i][j]];
                } else {
                    result = 0;
                }
            }
            for(int k = 0; k < MAT_SIZE; ++k){
                if (check_list[k] != 1){
                    result = 0;
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

