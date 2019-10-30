#ifndef FILEREADER_H_INCLUDED
#define FILEREADER_H_INCLUDED

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "level.h"

class FileReader {
    public:
        FileReader(std::string &_filename);
        ~FileReader();

        std::vector<Level*> read();

    private:
        std::string filename;
        std::vector<Level*> levels;

        bool isBoardRow(std::string row);

        const std::string BOARD_CHARS = "#@+$*. -_";
};

#endif // FILEREADER_H_INCLUDED
