#include "include/tests.h"

Tests::Tests() {
    testsTotal = 0;
    testsCorrect = 0;
}

void Tests::runTests() {
    runTest(testHungarianComplete, "Hungarian algorithm calculates distance on complete graph");
    runTest(testHungarianIncomplete, "Hungarian algorithm calculates distance on incomplete graph");
    runTest(testNumberOfLevels, "Load correct number of levels from test suite");
    runTest(testLevelSizesFromFile, "Correct level sizes are loaded from file");
    runTest(testLevelSizeFromString, "Correct level size is loaded from string");
    runTest(testBasicSolveFromFile, "Basic level is solved correctly from file");
    runTest(testBasicSolveFromString, "Basic level is solved correctly from string");

    std::cout << "\nTests passed: " << testsCorrect << "/" << testsTotal;
}

void Tests::runTest(std::function<void()> test, std::string desc) {
    testsTotal++;
    testsCorrect++;

    try {
        test();
    } catch(std::stringstream &s) {
        std::cerr << desc << s.str();
        testsCorrect--;
    }
}

void Tests::testHungarianComplete() {
    int **graph = new int*[4];
    for(int i = 0; i < 4; i++) {
        graph[i] = new int[4];
    }
    graph[0][0] = 14; graph[0][1] = 15; graph[0][2] = 8; graph[0][3] = 10;
    graph[1][0] = 11; graph[1][1] = 12; graph[1][2] = 5; graph[1][3] = 7;
    graph[2][0] = 5; graph[2][1] = 2; graph[2][2] = 11; graph[2][3] = 13;
    graph[3][0] = 4; graph[3][1] = 5; graph[3][2] = 10; graph[3][3] = 0;

    Hungarian h(4);
    int result = h.solve(graph);

    assertEquals(result, 21);

    for(int i = 0; i < 4; i++) {
        delete[] graph[i];
    }
    delete[] graph;
}

void Tests::testHungarianIncomplete() {
    int **graph = new int*[5];
    for(int i = 0; i < 5; i++) {
        graph[i] = new int[5];
    }
    graph[0][0] = 3; graph[0][1] = 4; graph[0][2] = -1; graph[0][3] = -1; graph[0][4] = -1;
    graph[1][0] = -1; graph[1][1] = 2; graph[1][2] = 4; graph[1][3] = -1; graph[1][4] = 5;
    graph[2][0] = 4; graph[2][1] = -1; graph[2][2] = -1; graph[2][3] = 3; graph[2][4] = -1;
    graph[3][0] = -1; graph[3][1] = -1; graph[3][2] = -1; graph[3][3] = 3; graph[3][4] = 4;
    graph[4][0] = -1; graph[4][1] = -1; graph[4][2] = -1; graph[4][3] = 5; graph[4][4] = 2;

    Hungarian h(5);
    int result = h.solve(graph);

    assertEquals(result, 17);

    for(int i = 0; i < 5; i++) {
        delete[] graph[i];
    }
    delete[] graph;
}

void Tests::testNumberOfLevels() {
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

    size_t levelNumber[20] = { 282, 100, 40, 40, 81, 135, 155, 50, 50, 50, 50, 50, 50, 50, 107, 163, 1623, 90, 54, 52 };

    for(int i = 0; i < 20; i++) {
        FileReader fr;
        std::vector<Level*> levels = fr.readFromFile(files[i]);

        assertEquals(levels.size(), levelNumber[i]);
    }
}

void Tests::testLevelSizesFromFile() {
    FileReader fr;
    std::string str = "test_suite/Microban_155.xsb";
    std::vector<Level*> levels = fr.readFromFile(str);

    int levelHeights[155] = { 7, 7, 6, 6, 7, 6, 8, 12, 7, 8, 8, 8, 9, 6, 7, 8, 7, 9, 8, 8, 6, 9, 7, 7, 7, 8, 7, 7, 9, 7, 7, 7, 7,
                                 6, 10, 5, 8, 7, 9, 6, 6, 8, 9, 3, 7, 8, 7, 8, 10, 7, 7, 8, 7, 8, 8, 6, 9, 7, 9, 10, 10, 6, 6, 9, 9,
                                 14, 8, 7, 8, 10, 9, 11, 10, 8, 7, 11, 7, 8, 6, 11, 9, 8, 10, 10, 11, 8, 10, 12, 9, 7, 5, 10, 11, 8,
                                 8, 11, 10, 10, 10, 8, 13, 8, 8, 9, 11, 9, 8, 8, 13, 9, 12, 13, 13, 9, 8, 7, 10, 10, 10, 11, 11, 12,
                                 11, 9, 8, 7, 9, 8, 8, 7, 11, 8, 8, 10, 7, 8, 10, 11, 12, 9, 9, 9, 15, 12, 11, 9, 10, 7, 12, 8, 10,
                                 9, 10, 17, 17 };

    int levelWidths[155] = { 6, 6, 9, 8, 8, 12, 7, 8, 6, 11, 9, 9, 7, 7, 9, 10, 6, 7, 8, 9, 7, 7, 7, 7, 7, 6, 7, 7, 11, 6, 7, 7,
                                7, 9, 7, 15, 9, 10, 10, 7, 8, 7, 9, 5, 6, 7, 11, 8, 8, 10, 8, 6, 7, 12, 10, 7, 8, 7, 13, 10, 9, 13,
                                19, 10, 9, 9, 7, 10, 11, 8, 13, 10, 8, 11, 10, 10, 11, 11, 10, 10, 6, 8, 8, 12, 11, 9, 9, 11, 10,
                                10, 11, 15, 11, 9, 8, 11, 14, 16, 22, 8, 13, 11, 8, 9, 11, 10, 8, 15, 9, 9, 9, 10, 14, 11, 12, 10,
                                11, 10, 9, 11, 11, 11, 15, 14, 13, 12, 10, 11, 11, 12, 7, 10, 10, 12, 11, 9, 13, 12, 16, 14, 10, 9,
                                9, 12, 11, 9, 12, 15, 10, 15, 12, 15, 13, 29, 31 };

    for(int i = 0; i < levels.size(); i++) {
        assertEquals(levels[i]->getHeight(), levelHeights[i]);
        assertEquals(levels[i]->getWidth(), levelWidths[i]);
    }
}

void Tests::testLevelSizeFromString() {
    FileReader fr;
    std::string str = "      ###|##### #.#|#   ###.#|#   $ #.#|# $  $  #|#####@# #|    #   #|    #####";
    std::vector<Level*> levels = fr.readFromString(str);

    assertEquals(levels[0]->getHeight(), 8);
    assertEquals(levels[0]->getWidth(), 9);
}

void Tests::testBasicSolveFromFile() {
    FileReader fr;
    std::string str = "test_suite/Microban_155.xsb";
    std::vector<Level*> levels = fr.readFromFile(str);
    std::string expected = "R";

    AStar a(*levels[43]);
    std::string lurd = a.start(10);

    assertEquals(lurd, expected);
}

void Tests::testBasicSolveFromString() {
    FileReader fr;
    std::string str = "#####|#@$.#|#####";
    std::vector<Level*> levels = fr.readFromString(str);
    std::string expected = "R";

    AStar a(*levels[0]);
    std::string lurd = a.start(10);

    assertEquals(lurd, expected);
}
