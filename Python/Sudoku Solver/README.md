# The Game of Life

This is a cellular automaton devised by the British mathematician John Horton Conway. It is a zero-player game, meaning that the behavior is determined by the initial state, which is randomised in this implementation.
![Sudoku Solver Visualized](https://github.com/walking-machine/Projects/blob/master/Python/Sudoku%20Solver/sudoku.gif)\
Firstly the cells are initialized, each sell has a 10% chance of being a live cell (in white), otherwise it is treated as a dead sell (in black). Then the simulation follows the rules:
1) Any live cell with fewer than two live neighbours dies, as if by underpopulation.
2) Any live cell with two or three live neighbours lives on to the next generation.
3) Any live cell with more than three live neighbours dies, as if by overpopulation.
4) Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
\
\
*This implementation uses winbgi2 library, provided by [CCFD group at my faculty (MEiL)](http://ccfd.github.io/courses/)*
