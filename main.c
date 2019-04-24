#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAT_SIZE 9
#define PIPE_NUM 4
#define PROCESS_NUM 3

void fork_and_assign(pid_t *process, int fd[PIPE_NUM][2]);
int init_process(int i, int fd[PIPE_NUM][2]);
void read_from_terminal_and_write_to_fd(int fd[PIPE_NUM][2]);
void read_from_file_and_write_to_fd(char *filename, int fd[PIPE_NUM][2]);
void check_sudoku(int fd[PIPE_NUM][2], char* filePath);
int read_from_result_pipe(int fd);
void print_matrix(int** mat);
void writeToPipe(int fd, int** matrix);

int main(int argc, char *argv[]) {
    pid_t process[PROCESS_NUM];
    int fd[PIPE_NUM][2];
    int choice;
    char* filePath; // = "/home/nikita/CLionProjects/my_module/demo.txt"; // debugging purposes 

    //init pipe for each file
    for (int i = 0; i < PIPE_NUM; ++i) {
        if (pipe(fd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        } else {
            printf("fd %d: %d , %d   ", i, fd[i][0], fd[i][1]);
        }
    }

    printf("\nEnter you're choice of input, 1 - file, 2 - terminal: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1 :
            scanf("%s", &filePath);
            read_from_file_and_write_to_fd(filePath, fd);
            break;

        case 2 :
            read_from_terminal_and_write_to_fd(fd);
            break;

        default :
            printf("Wrong input.");
            break;
    }

    fork_and_assign(process, fd);

    for (int i = 0; i < PROCESS_NUM; i++) {
        wait(NULL);
    }

    check_sudoku(fd, filePath);

    return 0;
}

void fork_and_assign(pid_t *process, int fd[PIPE_NUM][2]){
    for (int i = 0; i < PROCESS_NUM; ++i) {
        process[i] = fork();

        if (process[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);

        } else if (process[i] == 0) {
            close(fd[i][1]);  // tried without it, didn't work either
        //    printf("process %d launch\n", getpid());
            if(init_process(i, fd) == -1){
                fprintf(stderr, "Init process failed.");
                exit(1);
            }

        } else {
            continue;
        }
    }
}

int init_process(int i, int fd[PIPE_NUM][2]){
    char* execFile = "/home/nikita/CLionProjects/my_module/checkMatrix";
    char* part_to_check;
    char fdRead[12], fdWrite[12];

    if (i == 0) {
        part_to_check = "rows";
    } else if (i == 1) {
        part_to_check = "cols";
    } else {
        part_to_check = "subMat";
    }

    sprintf(fdRead,"%i",fd[i][0]);
    sprintf(fdWrite,"%i",fd[PIPE_NUM-1][1]);

    char* args[] = {execFile, part_to_check, fdRead, fdWrite,  NULL};
    return execvp(args[0], args);
}

void read_from_file_and_write_to_fd(char *filename, int fd[PIPE_NUM][2]){
    int i, j;
    int **mat = (int **) malloc(MAT_SIZE * sizeof(int*));
    for (i = 0; i < MAT_SIZE; ++i)
        mat[i] = (int *) malloc(MAT_SIZE * sizeof(int));
    char c;

    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL)
        fprintf(stderr, "File is missing.");

    for (i = 0; i < MAT_SIZE; ++i) {
        for (j = 0; j < MAT_SIZE; ++j) {
            fscanf(file, "%d%c", &mat[i][j], &c);
        }
    }

    print_matrix(mat);

    fclose(file);

    for (i = 0; i < PROCESS_NUM; ++i) {
        writeToPipe(fd[i][1], mat);
        close(fd[i][1]);
    }
}

void read_from_terminal_and_write_to_fd(int fd[PIPE_NUM][2]){
    int i, j;
    int **mat = (int **)malloc(MAT_SIZE * sizeof(int*));
    for(i = 0; i < MAT_SIZE; ++i)
        mat[i] = (int *)malloc(MAT_SIZE * sizeof(int));
    char c;


    for(i = 0; i < MAT_SIZE; ++i) {
        printf("\nEnter row number %d (num by num):\n", i+1);

        for(j = 0; j < MAT_SIZE; ++j) {
            scanf("%d%c", &mat[i][j], &c);
        }
    }

    print_matrix(mat);

    for (i = 0; i < PROCESS_NUM; ++i) {
        writeToPipe(fd[i][1], mat);
        close(fd[i][1]);
    }
}

void writeToPipe(int fd, int **matrix) {
    int i, j;
    for (i = 0; i < MAT_SIZE; ++i) {
        for (j = 0; j < MAT_SIZE; ++j) {
            write(fd, &matrix[i][j], sizeof(int));
        }
    }
}

void check_sudoku(int fd[PIPE_NUM][2], char *filePath) {
    // Read children answers
    int result = read_from_result_pipe(fd[PIPE_NUM - 1][0]); //Getting the results checks from children - Syntax ERROR

    if (result == 3) {
        printf("\nSudoku at file '%s' is legal\n", filePath);
    } else {
        printf("\nSudoku at file '%s' is not legal\n", filePath);
    }
}

int read_from_result_pipe(int fd) {
    int i, result = 0, tmp = 0;
    for (i = 0; i < PROCESS_NUM; i++) {
        read(fd, &tmp, sizeof(int));
//        printf("result: %d\n", tmp);
        if (tmp > 0)
            result++;
    }
    close(fd);
    return result;
}

void print_matrix(int** mat) {
    printf("\nYour matrix is:\n");
    for (int i = 0; i < MAT_SIZE; i++) {
        for (int j = 0; j < MAT_SIZE; j++)
            printf("%d ", mat[i][j]);
        printf("\n");
    }
    printf("\n");
}
