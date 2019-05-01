# SUDOKU 
## A multi process sudoku checking program.

Sudoku is a logic-based, combinatorial number-placement puzzle. 
The objective is to fill a 9×9 grid with digits so that each column, each row, and each of the nine 3×3 subgrids that compose the grid 
(also called "boxes", "blocks", or "regions") contain all of the digits from 1 to 9. 
The puzzle setter provides a partially completed grid, which for a well-posed puzzle has a single solution.

The app's purpose is to decide if a given matrix is valid sudoku solution.

## Instructions:
To run with gcc compiler:

1. Open linux terminal in the relevant directory.

1. Type: gcc -o sudoku main.c (for the multi-thread version add to the line [-pthread]) (this will produce a program called sudoku)

1. To run it: ./sudoku <filename> ( [filname] optional - a text file with a sudoku matrix)
  
1. if no file was given you will have to type in the matrix num by num inside the terminal.


## Implementations

### used_pipes_execvp

Uses 3 child processes and a pipe for each one to send them the data, execvp to run another app on each one.

### used_shared_mem

Uses 3 child processes and a shared memory.

### used_multi_threads

Uses 3 threads. Each one is responsible for a different part.

### used_threads_with_mutex

Uses 3 threads that are responsible for the whole test, have access to the same data.
Synchronization was taken care with mutex and condition.
