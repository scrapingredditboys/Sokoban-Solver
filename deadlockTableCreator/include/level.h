#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>

class Level {
    public:
        Level(std::vector<std::string> &boardData);
        ~Level();

        int getWidth() { return width; }
        int getHeight() { return height; }
        char *getBoard() { return board; }
        int getBoxCount() { return boxCount; }
        int getGoalCount() { return goalCount; }
        int getSize() { return width * height; }
        void print();
        void updateCounts();

    private:
        int width;
        int height;
        char *board;
        int boxCount;
        int goalCount;

        int calculateWidth(std::vector<std::string> &levelData);
        int calculateHeight(std::vector<std::string> &levelData);
        void normalizeLevelData(std::vector<std::string> &levelData);
};

#endif // LEVEL_H_INCLUDED
