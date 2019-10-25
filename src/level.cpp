#include "include/level.h"

Level::Level(std::vector<std::string> &levelData) {
    width = calculateWidth(levelData);
    height = calculateHeight(levelData);

    normalizeLevelData(levelData);

    board = new char*[height];

    for(int i = 0; i < height; i++) {
        board[i] = new char[width];
        for(int j = 0; j < width; j++) {
            board[i][j] = levelData[i][j];
        }
    }
}

Level::~Level() {
    for(int i = 0; i < height; i++) {
        delete[] board[i];
    }
    delete[] board;
}

void Level::print() {
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            std::cout << board[i][j];
        }
        std::cout << std::endl;
    }
}

int Level::calculateWidth(std::vector<std::string> &levelData) {
    int maxWidth = levelData[0].length();
    for(int i = 1; i < levelData.size(); i++) {
        maxWidth = (maxWidth < levelData[i].length()) ? levelData[i].length() : maxWidth;
    }
    return maxWidth;
}

int Level::calculateHeight(std::vector<std::string> &levelData) {
    return levelData.size();
}

void Level::normalizeLevelData(std::vector<std::string> &levelData) {
    for(int i = 0; i < levelData.size(); i++) {
        for(int j = 0; j < levelData[i].length(); j++) {
            if(levelData[i][j] == '-' || levelData[i][j] == '_') {
                levelData[i][j] = ' ';
            }
        }
        int missingSpaces = width - levelData[i].length();
        for(int j = 0; j < missingSpaces; j++) {
            levelData[i] += " ";
        }
    }
}
