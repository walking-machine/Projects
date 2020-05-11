#include <iostream>
#include <fstream>
#include "Board.h"
#include "sl.h"
using namespace std;

int main() {
    //initialization
    const int width = 64;
    start_graphics(width);

    //reading from file
    fstream file;
    file.open("../boards/expert.txt");
    if (!file) {
        cerr << "Unable to open file";
        exit(1);
    }
    char c;
    short table[9][9];
    short i = 0;
    while (file>>c){
        int x = i/9;
        int y = i%9;
        if(c=='.')
            table[x][y] = 0;
        else if(isdigit(c))
            table[x][y] = c - '0';
        else{
            cerr<<"Invalid character in input file";
            exit(1);
        }
        i++;
    }
    file.close();
    if(i < 81){
        cerr<<"Not enough characters";
        exit(1);
    }

    //Board object creation
    Board b(table, width);

    //Waiting for Enter to start (or ESC to exit)
    while (!slGetKey(SL_KEY_ENTER)){
        b.draw_table();
    }

    //solving
    solve(b);

    //Waiting for ESC key to finish
    while(!slGetKey(SL_KEY_ESCAPE)){
        b.draw_table();
    }
    slClose();
    return 0;
}
