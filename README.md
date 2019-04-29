<h1> SUDOKU </h1>
<h3> A multi process sudoku checking program. </h3>

Sudoku is a logic-based, combinatorial number-placement puzzle. 
The objective is to fill a 9×9 grid with digits so that each column, each row, and each of the nine 3×3 subgrids that compose the grid 
(also called "boxes", "blocks", or "regions") contain all of the digits from 1 to 9. 
The puzzle setter provides a partially completed grid, which for a well-posed puzzle has a single solution.

The app's purpose is to decide if a given matrix is valid sudoku solution.

<h4> Instructions: </h4>
To run with gcc compiler:
1. Open linux terminal in the relevant directory.
2. Type: gcc -o sudoku main.c (this will produce a program called sudoku)
3. To run it: ./sudoku <filename> (filname [optional] - a text file with a sudoku matrix)
4. if no file was given you will have to type in the matrix num by num inside the terminal.

<h1> </h1>

<h6> used_pipes_execvp </h6>
Uses 3 child prcesses and a pipe for each one to send them the data, execvp to run another app on each one.

<h6> used_shared_mem </h6>
Uses 3 child processes and a shared memory.

<h6> used_multi_threads </h6>
Uses 3 threads. Each one is responsible for a different part.

<h6> used_threads_with_mutex </h6>
Uses 3 threads that are responsible for the whole test, have access to the same data.
synchronization was taken care with mutex and condition.
