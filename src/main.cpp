#include <iostream>
#include <string>

#include "include/fileReader.h"
#include "include/level.h"
#include "include/tests.h"

int main()
{
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
}
