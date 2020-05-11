#include <iostream>
#include "Board.h"
#include "sl.h"
#include <windows.h>
using namespace std;

Board::Board(const short (&arr)[9][9], int width) {
    copy(&arr[0][0], &arr[0][0]+81, &table[0][0]);
    size = width;
}

void Board::draw_table() {
    //To prevent "Not responding" state
    if(slGetKey(SL_KEY_ESCAPE)) {
        slClose();
        exit(1);
    }

    //Drawing
    slSetBackColor(0, 0, 0);
    slSetForeColor(0, 255, 0, 255);

    for(int i = 1; i < 9; i++){
        slLine(size*i, 0, size*i, size*9);
        slLine(0, size*i, size*9, size*i);
    }

    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++){
            slRectangleOutline(size*3*i + 3*size/2, size*3*j + 3*size/2, size*3 - 4, size*3 - 4);
        }

    for(int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            char const *text = to_string(table[i][j]).c_str();
            int width = static_cast<int>(slGetTextWidth(text));
            int height = static_cast<int>(slGetTextHeight(text));
            int x_offset = (size - width) / 2;
            int y_offset = (size - height) / 2;
            slText(j * size + x_offset, size * (8 - i) + y_offset, text);
        }
    }

    //Ensuring framerate is 20fps
    double dt = slGetDeltaTime();
    double goal = 0.05;
    if (goal>dt) Sleep((goal-dt)*1000);

    slRender();
}

void start_graphics(int width){
    slWindow(width*9, width*9, "Sudoku", false);
    int font = slLoadFont("C:\\WINDOWS\\FONTS\\BGOTHL.TTF");
    slSetFont(font, width/2);
    slSetBackColor(0, 0, 0);
    slRender();
}

bool is_valid(const short (&table)[9][9], int row, int col, short num){
    for(int i = 0; i < 9; i++){
        if(table[i][col]==num) return false;
        if(table[row][i]==num) return false;
    }

    int row_start = (row/3)*3;
    int col_start = (col/3)*3;
    for(int i = row_start; i < row_start+3; i++)
        for(int j = col_start; j < col_start+3; j++) {
            if (table[i][j]==num) return false;
        }
    return true;
}

bool solve(Board &board){
    for(int i = 0; i < 9; i++)
        for(int j = 0; j < 9; j++){
            if(board.table[i][j]==0){
                for(short p = 1; p < 10; p++){
                    if(is_valid(board.table, i, j, p)){
                        board.table[i][j] = p;
                        board.draw_table();
                        if(solve(board)) return true;
                    }
                }
                board.table[i][j] = 0;
                return false;
            }
        }
    return true;
}