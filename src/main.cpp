#include <iostream>
#include <stdlib.h>
#include <string>

#include "include/astar.h"
#include "include/fileReader.h"
#include "include/level.h"
#include "include/tests.h"

void testHungarian();

int main(int argc, char **argv)
{
    srand(42);

    if(argc > 1) {
        FileReader fr;
        std::string str(argv[1]);
        std::vector<Level*> levels = fr.readFromString(str);
        AStar astar = AStar(*(levels[0]));

        double timeLimit = 60;
        char *p;

        if(argc > 2) {
            timeLimit = strtol(argv[2], &p, 10)/1000.0;
        }

        std::cout << astar.start(timeLimit);

        return 0;
    }

    /*FileReader fr;
    std::string filename = "test_suite/Microban_155.xsb";
    //std::string filename = "test_suite/XSokoban_90.xsb";
    std::vector<Level*> levels = fr.readFromFile(filename);
    for(int i = 0; i < levels.size(); i++) {
        srand(42);
        std::cout << "Level " << i + 1 << ": ";
        AStar a = AStar(*(levels[i]));
        a.start(60);
    }

    return 0;*/

    Tests t;
    t.runTests();

    return 0;

    /*int file = 6;

    FileReader fr;
    std::vector<Level*> levels = fr.readFromFile(files[file]);

    //std::cout << "Solving " << files[file] << "\n";
    for(int level = 0; level < 1; level++) {
        //std::cout << "Level " << level + 1 << ": ";
        AStar astar = AStar(*(levels[level]));
        astar.start(60);
    }

    return 0;*/
}
