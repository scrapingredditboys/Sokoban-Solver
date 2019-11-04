#include <stdlib.h>
#include <iostream>
#include <string>

#include "include/astar.h"
#include "include/fileReader.h"
#include "include/level.h"
#include "include/tests.h"

void testHungarian();

int main()
{
    srand(42);

    std::string files[20] = {
        "test_suite/Aymeric_Du_Peloux_282.xsb",
        "test_suite/Grigr2001_100.xsb",
        "test_suite/Grigr2002_40.xsb",
        "test_suite/GrigrSpecial_40.xsb",
        "test_suite/Holland_81.xsb",
        "test_suite/Microban II_135.xsb",
        "test_suite/Microban_155.xsb",
        "test_suite/Sasquatch II_50.xsb",
        "test_suite/Sasquatch_50.xsb",
        "test_suite/Sasquatch_III_50.xsb",
        "test_suite/Sasquatch_IV_50.xsb",
        "test_suite/Sasquatch_VII_50.xsb",
        "test_suite/Sasquatch_VI_50.xsb",
        "test_suite/Sasquatch_V_50.xsb",
        "test_suite/Sokevo_107.xsb",
        "test_suite/Sokhard_163.xsb",
        "test_suite/Sven_1623.xsb",
        "test_suite/XSokoban_90.xsb",
        "test_suite/Yoshio Murase_handmade_54.xsb",
        "test_suite/Yoshio_Murase_auto_generated_52.xsb"
    };

    int levelNumber[20] = { 282, 100, 40, 40, 81, 135, 155, 50, 50, 50, 50, 50, 50, 50, 107, 163, 1623, 90, 54, 52 };

    for(int i = 0; i < 20; i++) {
        FileReader fr = FileReader(files[i]);
        std::vector<Level*> levels = fr.read();

        assertNumberOfLevels(levels, levelNumber[i]);
    }

    testHungarian();

    int file = 6;

    FileReader fr = FileReader(files[file]);
    std::vector<Level*> levels = fr.read();

    std::cout << "Solving " << files[file] << "\n";
    for(int level = 0; level < levelNumber[file]; level++) {
        std::cout << "Level " << level + 1 << ": ";
        AStar astar = AStar(*(levels[level]));
        astar.start(60);
    }
}

void testHungarian() {
    int **graph1 = new int*[4];
    for(int i = 0; i < 4; i++) {
        graph1[i] = new int[4];
    }
    graph1[0][0] = 14; graph1[0][1] = 15; graph1[0][2] = 8; graph1[0][3] = 10;
    graph1[1][0] = 11; graph1[1][1] = 12; graph1[1][2] = 5; graph1[1][3] = 7;
    graph1[2][0] = 5; graph1[2][1] = 2; graph1[2][2] = 11; graph1[2][3] = 13;
    graph1[3][0] = 4; graph1[3][1] = 5; graph1[3][2] = 10; graph1[3][3] = 0;

    assertMinimumCostMatching(graph1, 4, 21);

    for(int i = 0; i < 4; i++) {
        delete[] graph1[i];
    }
    delete[] graph1;


    int **graph2 = new int*[5];
    for(int i = 0; i < 5; i++) {
        graph2[i] = new int[5];
    }
    graph2[0][0] = 3; graph2[0][1] = 4; graph2[0][2] = -1; graph2[0][3] = -1; graph2[0][4] = -1;
    graph2[1][0] = -1; graph2[1][1] = 2; graph2[1][2] = 4; graph2[1][3] = -1; graph2[1][4] = 5;
    graph2[2][0] = 4; graph2[2][1] = -1; graph2[2][2] = -1; graph2[2][3] = 3; graph2[2][4] = -1;
    graph2[3][0] = -1; graph2[3][1] = -1; graph2[3][2] = -1; graph2[3][3] = 3; graph2[3][4] = 4;
    graph2[4][0] = -1; graph2[4][1] = -1; graph2[4][2] = -1; graph2[4][3] = 5; graph2[4][4] = 2;

    assertMinimumCostMatching(graph2, 5, 17);

    for(int i = 0; i < 5; i++) {
        delete[] graph2[i];
    }
    delete[] graph2;
}
