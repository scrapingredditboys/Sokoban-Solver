#ifndef FILEREADER_H_INCLUDED
#define FILEREADER_H_INCLUDED

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "level.h"

class FileReader {
    public:
        //FileReader();
        ~FileReader();

        std::vector<Level*> readFromFile(std::string &filename);
        std::vector<Level*> readFromString(std::string &str);

    private:
        std::vector<Level*> levels;

        bool isBoardRow(std::string &row);

        const std::string BOARD_CHARS = "#@+$*. -_";
};

#endif // FILEREADER_H_INCLUDED
