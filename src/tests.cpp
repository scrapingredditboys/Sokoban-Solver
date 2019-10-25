#include "include/tests.h"

void assertNumberOfLevels(std::vector<Level*> &levels, int number) {
    if(levels.size() != number) {
        std::cerr << "Invalid number of levels!\n  Expected: " << number << "\n  Got: " << levels.size() << std::endl;
    }
}

void assertLevelSize(Level &level, int width, int height) {
    if(level.getWidth() != width || level.getHeight() != height) {
        std::cerr << "Invalid level size!\n  Expected: " << level.getWidth() << " x " << level.getHeight() <<
                     "\n  Got: " << width << " x " << height << std::endl;
    }
}
