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
        std::string mode = "ida*";
        std::vector<Level*> levels = fr.readFromString(str);

        double timeLimit = 60;
        char *p;

        if(argc > 2) {
            timeLimit = strtol(argv[2], &p, 10)/1000.0;
        }
        if(argc > 3) {
            mode = argv[3];
        }

        AStar astar = AStar(*(levels[0]), mode);
        std::cout << astar.start(timeLimit);

        return 0;
    }

    Tests t;
    t.runTests();

    return 0;
}
