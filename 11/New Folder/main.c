#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAT_SIZE 9
#define PIPE_NUM 4
#define PROCESS_NUM 3

void fork_and_assign(pid_t *process, int fd[PIPE_NUM][2]);
int init_process(int i, int fd[PIPE_NUM][2]);
void read_from_terminal_and_write_to_fd(int fd[PIPE_NUM][2]);
void read_from_file_and_write_to_fd(char *filename, int fd[PIPE_NUM][2]);
void check_sudoku();

int main(int argc, char *argv[]) {
    pid_t process[PROCESS_NUM];
    int fd[PIPE_NUM][2];
    int choice;
    char* filePath = "/home/nikita/CLionProjects/soduku_check/wrong1";

    //init pipe for each file
    for (int i = 0; i < PIPE_NUM; ++i) {
        if (pipe(fd[i]) == -1) {
            fprintf(stderr, "Pipe failed");
        } else {
            printf("fd %d: %d , %d   ", i, fd[i][0], fd[i][1]);
        }
    }

    printf("\nEnter you're choice of input, 1 - file, 2 - terminal: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1 :
      //      scanf("%s", &filePath);
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

    check_sudoku();

    return 0;
}

void fork_and_assign(pid_t *process, int fd[PIPE_NUM][2]){
    for (int i = 0; i < PROCESS_NUM; ++i) {
        process[i] = fork();

        if (process[i] < 0) {
            fprintf(stderr, "fork failed\n");
            exit(1);

        } else if (process[i] == 0) {
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
    char* execFile = "checkMatrix";
    char* part_to_check;
    if (i == 0) {
        part_to_check = "rows";
    } else if (i == 1) {
        part_to_check = "cols";
    } else {
        part_to_check = "subMat";
    }

    close(fd[i][1]);
    close(fd[PIPE_NUM-1][1]);
    close(fd[PIPE_NUM-1][0]);
    dup2(fd[i][0], STDIN_FILENO);
    dup2(fd[PIPE_NUM-1][1], STDIN_FILENO);

    char* args[] = {execFile, part_to_check, NULL};
    return execvp(args[0], args);
}

void read_from_file_and_write_to_fd(char *filename, int fd[PIPE_NUM][2]){
    int **mat = (int **)malloc(MAT_SIZE * sizeof(int*));
    for(int i = 0; i < MAT_SIZE; ++i)
        mat[i] = (int *)malloc(MAT_SIZE * sizeof(int));

    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL)
        fprintf(stderr, "File is missing.");

    for (int i = 0; i < MAT_SIZE; ++i) {
        for(int j = 0; j < MAT_SIZE; ++j) {
            fscanf(file, "%d", &mat[i][j]);
        }
    }

    fclose(file);
    for (int i = 0; i < PROCESS_NUM; ++i){
        write (fd[i][1], mat, MAT_SIZE * MAT_SIZE * sizeof(int));
        close(fd[i][0]);
        close(fd[i][1]);
    }
}

void read_from_terminal_and_write_to_fd(int fd[PIPE_NUM][2]){
    int **mat = (int **)malloc(MAT_SIZE * sizeof(int*));
    for(int i = 0; i < MAT_SIZE; ++i)
        mat[i] = (int *)malloc(MAT_SIZE * sizeof(int));


    for(int i = 0; i < MAT_SIZE; ++i) {
        printf("\nEnter row number %d (num by num):\n", i+1);

        for(int j = 0; j < MAT_SIZE; ++j) {
            scanf("%d", &mat[i][j]);
        }
    }

    for (int i = 0; i < PROCESS_NUM; ++i){
        write (fd[i][0], mat, MAT_SIZE * MAT_SIZE * sizeof(int));
        close(fd[i][0]);
        close(fd[i][1]);
    }
}

void check_sudoku() {
    char buf[256];
    int answer1, answer2, answer3;

    // Read children answers
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        fprintf(stderr, "Illegal answer from children.\n");
        exit(1);
    }

    sscanf(buf, "%d%d%d", &answer1, &answer2, &answer3);
    int sudoku = answer1 + answer2 + answer3;

    if (sudoku == 3){
        printf("\nFILENAME is legal\n");
    } else {
        printf("\nFILENAME is not legal\n");
    }
}
