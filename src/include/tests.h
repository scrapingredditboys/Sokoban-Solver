#ifndef TESTS_H_INCLUDED
#define TESTS_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>

#include "level.h"

void assertNumberOfLevels(std::vector<Level*> &levels, int number);
void assertLevelSize(Level &level, int width, int height);

#endif // TESTS_H_INCLUDED
