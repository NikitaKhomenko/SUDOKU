#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define MAT_SIZE 9
#define PROCESS_NUM 3
#define SOLUTION_SIZE (MAT_SIZE * MAT_SIZE * sizeof(int))
#define STATUS_SIZE (PROCESS_NUM * sizeof(int))
#define MEM_LENGTH (SOLUTION_SIZE + STATUS_SIZE)

typedef struct shared {
    int solution[MAT_SIZE][MAT_SIZE];
    int status[3];
} SHARED;

void fork_and_assign(pid_t *process, SHARED *shared_struct_obj);
void init_process(int i, SHARED *shared_struct_obj);
void read_from_terminal_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], SHARED *shared_struct_obj);
void read_from_file_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], char *filename, SHARED *shared_struct_obj);
void check_sudoku(char* filePath, SHARED *shared_struct_obj);
void print_matrix(int matrix[MAT_SIZE][MAT_SIZE]);
int check_rows(int matrix[MAT_SIZE][MAT_SIZE]);
int check_cols(int matrix[MAT_SIZE][MAT_SIZE]);
int check_subMat(int matrix[MAT_SIZE][MAT_SIZE]);
void wait_for_children();


int main(int argc, char *argv[]) {
    int matrix[MAT_SIZE][MAT_SIZE];
    pid_t process[PROCESS_NUM];
    int choice;
    int* addr;
    char* filePath; // "/home/nikita/CLionProjects/my_module/demo.txt"; // debugging purposes

    SHARED *shared_struct_obj = mmap(NULL, MEM_LENGTH, PROT_READ | PROT_WRITE,
                                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("Mmap failed\n");
        exit(EXIT_FAILURE);
    }

    printf("\nEnter you're choice of input, 1 - file, 2 - terminal: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1 :
            scanf("%s", &filePath);
            read_from_file_and_write_matrix(matrix, filePath, shared_struct_obj);
            break;

        case 2 :
            read_from_terminal_and_write_matrix(matrix, shared_struct_obj);
            break;

        default :
            printf("Wrong input.");
            break;
    }

    fork_and_assign(process, shared_struct_obj);
    wait_for_children();
    check_sudoku(filePath, shared_struct_obj);

    return 0;
}

void fork_and_assign(pid_t *process, SHARED *shared_struct_obj) {
    for (int i = 0; i < PROCESS_NUM; i++) {
        process[i] = fork();

        if (process[i] == 0) {
            init_process(i, shared_struct_obj);

        } else if (process[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);

        } else {
            continue;
        }
    }
}

void init_process(int i, SHARED *shared_struct_obj) {
    int matrix[MAT_SIZE][MAT_SIZE];
    int result = 0;

    memcpy(matrix, shared_struct_obj->solution, SOLUTION_SIZE);

    if (i == 0) {
        result = check_rows(matrix);
    } else if (i == 1) {
        result = check_cols(matrix);
    } else {
        result = check_subMat(matrix);
    }

    shared_struct_obj->status[i] = result;
    exit(EXIT_SUCCESS);
}

void read_from_file_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], char *filename, SHARED *shared_struct_obj){
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
    memcpy(shared_struct_obj->solution, matrix, SOLUTION_SIZE);

    fclose(file);
}

void read_from_terminal_and_write_matrix(int matrix[MAT_SIZE][MAT_SIZE], SHARED *shared_struct_obj){
    int i, j;
    char c;

    for(i = 0; i < MAT_SIZE; ++i) {
        printf("\nEnter row number %d (num by num):\n", i+1);

        for(j = 0; j < MAT_SIZE; ++j) {
            scanf("%d%c", &matrix[i][j], &c);
        }
    }

    print_matrix(matrix);
    memcpy(shared_struct_obj->solution, matrix, SOLUTION_SIZE);
}

void check_sudoku(char *filePath, SHARED *shared_struct_obj) {
    int final_result = 0;
    for(int i = 0; i < PROCESS_NUM; ++i){
        final_result += shared_struct_obj->status[i];
    }

    if (final_result == 3) {
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

void wait_for_children(){
    for (int i = 0; i < PROCESS_NUM; i++) {
        wait(NULL);
    }
}



// The algorithm: we keep a checking list, each value in the row/col/subMat represent an index in the checklist.
// We move through each row/col/subMat and upgrade the index in the checklist accordingly.
// If we find a value not in the range of 1-9 we disqualify the sudoku.
// If after iterating through each row/col/subMat we find any index in the checklist that it's not 1 we disqualify the sudoku.

int check_rows(int matrix[MAT_SIZE][MAT_SIZE]){
    printf("checking rows\n");
    int result = 1;
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
    return  result;
}

int check_cols(int matrix[MAT_SIZE][MAT_SIZE]){
    printf("checking cols\n");
    int result = 1;
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
    return result;
}

int check_subMat(int matrix[MAT_SIZE][MAT_SIZE]){
    printf("checking sub matrix\n");
    int result = 1;
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
    return result;
}

