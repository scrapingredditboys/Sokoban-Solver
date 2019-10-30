#include "include/level.h"

Level::Level(std::vector<std::string> &levelData) {
    width = calculateWidth(levelData);
    height = calculateHeight(levelData);
    boxCount = 0;

    normalizeLevelData(levelData);

    board = new char[width * height];

    for(int i = 0; i < height * width; i++) {
        board[i] = levelData[i / width][i % width];
    }
}

Level::~Level() {
    delete[] board;
}

void Level::print() {
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            std::cout << board[i * width + j];
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
            else if(levelData[i][j] == '$' || levelData[i][j] == '*') {
                boxCount++;
            }
        }
        int missingSpaces = width - levelData[i].length();
        for(int j = 0; j < missingSpaces; j++) {
            levelData[i] += " ";
        }
    }
}
