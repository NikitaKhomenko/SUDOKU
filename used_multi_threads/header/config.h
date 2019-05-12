//
// Created by nikita on 5/12/19.
//

#ifndef SUDOKU_THREAD1_CONFIG_H
#define SUDOKU_THREAD1_CONFIG_H

#define MAT_SIZE 9
#define SOLUTION_SIZE (MAT_SIZE * MAT_SIZE * sizeof(int))

typedef struct gameState {
    int solution[MAT_SIZE][MAT_SIZE];
    int status_rows[MAT_SIZE];
    int status_cols[MAT_SIZE];
    int status_subMat[MAT_SIZE];
}

GAMESTATE;typedef struct shared {
    GAMESTATE* gameState_to_share;
    int index;
} SHARED;

#endif //SUDOKU_THREAD1_CONFIG_H
