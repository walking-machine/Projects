#ifndef SUDOKU_SOLVER_C___BOARD_H
#define SUDOKU_SOLVER_C___BOARD_H

//Class storing board current state and capable of drawing it
class Board {
public:
    short table[9][9];
    void draw_table();
    Board(const short (&arr)[9][9], int width);

private:
    int size;
};

//Main solving function
bool solve(Board &board);

//Helper function to determine if certain number can be placed in cell
bool is_valid(const short (&table)[9][9], int row, int col, short num);

//Function for initializing graphics window and some parameters
void start_graphics(int width);

#endif SUDOKU_SOLVER_C___BOARD_H