#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

#include "include/astar.h"
#include "include/deadlockTable.h"
#include "include/fileReader.h"

using namespace std;

#define TILES 5

const char tiles[TILES] = {'$', '*', '#', '.', ' '};

void findDeadlocks(int, int, int, char*, Level*, std::ofstream&, DeadlockTable*);
int deadlocks = 0;

int main()
{
    std::ofstream file("deadlockTable.txt");

    int height = 3;
    int width = 3;

    std::string str = "";
    for(int i = 0; i < width + 4; i++) str += "#";
    str += "|#+";
    for(int i = 0; i < width; i++) str += ".";
    for(int i = 0; i < height; i++) {
        str += ".#|#.";
        for(int j = 0; j < width; j++) str += " ";
    }
    str += ".#|#";
    for(int i = 0; i < width + 2; i++) str += ".";
    str += "#|";
    for(int i = 0; i < width + 4; i++) str += "#";


    FileReader fr;
    std::vector<Level*> levels = fr.readFromString(str);

    char* board = levels[0]->getBoard();
    DeadlockTable deadlockTable(3, 2, levels[0]->getHeight(), levels[0]->getWidth(), "deadlockTable3x2.txt");

    findDeadlocks(height, width, 0, board, levels[0], file, &deadlockTable);
    std::cout << "Found " << deadlocks << " deadlocks.";

    return 0;
}

void findDeadlocks(int height, int width, int i, char* board, Level* level, std::ofstream &file, DeadlockTable *dlt) {
    if(i == width * height) {
        level->updateCounts();
        AStar a(*level, dlt);
        if(!a.start()) {
            for(int j = 0; j < i; j++) {
                int x = 2 + j % width;
                int y = 2 + j / width;
                int t = y * level->getWidth() + x;
                file << board[t];
            }
            file << '\n';
            deadlocks++;
        }
        return;
    }

    int x = 2 + i % width;
    int y = 2 + i / width;
    int t = y * level->getWidth() + x;
    for(int j = 0; j < TILES; j++) {
        if(i == 0) std::cout << j * 625 << "/3125\n";
        else if(i == 1) std::cout << "  " << j * 125 << "/625\n";
        else if(i == 2) std::cout << "    " << j * 25 << "/125\n";
        else if(i == 3) std::cout << "      " << j * 5 << "/25\n";
        else if(i == 4) std::cout << "        " << j << "/5\n";
        board[t] = tiles[j];
        findDeadlocks(height, width, i + 1, board, level, file, dlt);
    }
}
