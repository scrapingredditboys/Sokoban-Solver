#include "include/deadlockTable.h"

DeadlockTable::DeadlockTable(int _height, int _width, int _levelHeight, int _levelWidth, int _boxCount, std::string filename) {
    height = _height;
    width = _width;
    levelHeight = _levelHeight;
    levelWidth = _levelWidth;
    levelSize = levelHeight * levelWidth;
    boxCount = _boxCount;

    int tableSize = power(TILES_NUM, width * height);
    table = new bool[tableSize];
    for(int i = 0; i < tableSize; i++) {
        table[i] = false;
    }

    std::ifstream file(filename);
    std::string line;
    while(std::getline(file, line)) {
        addDeadlock(line);
    }
}

DeadlockTable::~DeadlockTable() {
    delete[] table;
}

void DeadlockTable::addDeadlock(std::string &str) {
    int index = 0;
    for(int i = 0; i < str.length(); i++) {
        index += power(TILES_NUM, str.length() - i - 1) * TILES.find(str[i]);
    }
    table[index] = true;
}

bool DeadlockTable::isDeadlock(char *board, State &state, int startIndex) {
    char newBoard[levelSize];
    for(int i = 0; i < levelSize; i++) {
        newBoard[i] = board[i];
    }
    for(int i = 0; i < boxCount; i++) {
        char t = newBoard[state.boxes[i]];
        if(t == ' ') newBoard[state.boxes[i]] = '$';
        else if(t == '.') newBoard[state.boxes[i]] = '*';
    }
    for(int i = 0; i < height * width; i++) {
        int beginIndex = startIndex - (i % width) - (1 /width) * levelWidth;
        int x = beginIndex % levelWidth;
        int y = beginIndex / levelWidth;
        if(x < 0 || x > levelWidth - width) continue;
        if(y < 0 || y > levelHeight - height) continue;

        int index = 0;
        for(int j = 0; j < height * width; j++) {
            index += power(TILES_NUM, height * width - j - 1) * TILES.find(newBoard[beginIndex + (j % width) + (j / width) * levelWidth]);
        }
        if(table[index]) return true;
    }
    return false;
}

int DeadlockTable::power(int b, int e) {
    int res = 1;
    for(int i = 0; i < e; i++) {
        res *= b;
    }
    return res;
}
