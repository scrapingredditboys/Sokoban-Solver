#ifndef HUNGARIAN_H_INCLUDED
#define HUNGARIAN_H_INCLUDED

#include <iostream>
#include <queue>
#include <string.h>

class Hungarian {
    public:
        Hungarian(int _size);
        int solve(int **graph);

    private:
        int size;

        bool zeroRows(int **graph);
        bool zeroColumns(int **graph);
        bool maximizeLines(int **graph);
        int* maximumMatching(int **graph);
        bool augmentPath(int **graph, int box, bool *seen, int *matchToBox);
};

#endif // HUNGARIAN_H_INCLUDED
