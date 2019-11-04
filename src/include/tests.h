#ifndef TESTS_H_INCLUDED
#define TESTS_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>

#include "hungarian.h"
#include "level.h"

void assertNumberOfLevels(std::vector<Level*> &levels, int number);
void assertLevelSize(Level &level, int width, int height);
void assertMinimumCostMatching(int **graph, int size, int expected);

#endif // TESTS_H_INCLUDED
