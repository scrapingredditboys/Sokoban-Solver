#include "include/fileReader.h"

FileReader::~FileReader() {
    for(int i = 0; i < levels.size(); i++) {
        delete levels[i];
    }
}

std::vector<Level*> FileReader::readFromFile(std::string &filename) {
    std::ifstream file(filename);
    std::string row;
    std::vector<std::string> levelRows;

    bool inLevel = false;

    while(std::getline(file, row)) {
        if(isBoardRow(row)) {
            inLevel = true;
            levelRows.push_back(row);
        }
        else {
            if(inLevel) {
                levels.push_back(new Level(levelRows));
                levelRows.clear();
            }
            inLevel = false;
        }
    }
    if(inLevel) {
        levels.push_back(new Level(levelRows));
    }

    return levels;
}

std::vector<Level*> FileReader::readFromString(std::string &str) {
    std::vector<std::string> levelRows;
    int pos  = 0;
    while((pos = str.find("|")) != std::string::npos) {
        levelRows.push_back(str.substr(0, pos));
        str.erase(0, pos + 1);
    }
    levelRows.push_back(str);

    levels.push_back(new Level(levelRows));
    return levels;
}

bool FileReader::isBoardRow(std::string &row) {
    if(row.length() == 0) {
        return false;
    }

    bool hasOtherThanSpaceChar = false;
    for(int i = 0; i < row.length(); i++) {
        bool hasBoardChar = false;
        for(int j = 0; j < BOARD_CHARS.length(); j++) {
            if(row[i] == BOARD_CHARS[j]) {
                if(row[i] != ' ') {
                    hasOtherThanSpaceChar = true;
                }
                hasBoardChar = true;
                break;
            }
        }
        if(!hasBoardChar) {
            return false;
        }
    }
    if(!hasOtherThanSpaceChar) {
        return false;
    }

    return true;
}
