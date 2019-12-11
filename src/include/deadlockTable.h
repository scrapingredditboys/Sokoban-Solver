#ifndef DEADLOCKTABLE_H_INCLUDED
#define DEADLOCKTABLE_H_INCLUDED

#include <fstream>
#include <string>

#include "state.h"

#define TILES_NUM 5

class DeadlockTable {
    public:
        DeadlockTable(int _height, int _width, int _levelHeight, int _levelWidth, int _boxCount, std::string filename);
        ~DeadlockTable();
        bool isDeadlock(char *board, State &state, int startIndex);

    private:
        int height;
        int width;
        int levelHeight;
        int levelWidth;
        int levelSize;
        int boxCount;
        bool *table;

        void addDeadlock(std::string &str);
        int power(int b, int e);

        const std::string TILES = "#$*. ";
};

#endif // DEADLOCKTABLE_H_INCLUDED
