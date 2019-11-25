#ifndef TESTS_H_INCLUDED
#define TESTS_H_INCLUDED

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "astar.h"
#include "fileReader.h"
#include "hungarian.h"
#include "level.h"

class Tests {
    public:
        Tests();
        void runTests();

    private:
        int testsTotal;
        int testsCorrect;

        static void testHungarianComplete();
        static void testHungarianIncomplete();
        static void testNumberOfLevels();
        static void testLevelSizesFromFile();
        static void testLevelSizeFromString();
        static void testBasicSolveFromFile();
        static void testBasicSolveFromString();

        void runTest(std::function<void()> test, std::string desc);

        template<typename T>
        static void assertEquals(T a, T b);
};

template<typename T>
void Tests::assertEquals(T value, T expected) {
    if(expected == value) {
        return;
    }
    std::stringstream s;
    s << "\n  Expected: " << expected << "\n  Got: " << value << "\n";
    throw s;
}

#endif // TESTS_H_INCLUDED
