#ifndef ASTAR_H_INCLUDED
#define ASTAR_H_INCLUDED

#include <ctime>
#include <iostream>
#include <queue>
#include <string.h>
#include <vector>

#include "level.h"
#include "state.h"

// In readlity, this is the IDA* approach, not A*

class AStar {
    public:
        AStar(Level &_level);
        ~AStar();
        void start(double timeLimit);

    private:
        Level level;
        int *goals;

        State* getInitialState();
        void calculateH(State &state);
        int** getBipartiteGraph(State &state);
        int getGoalIndex(int position);
        void playerBFS(bool *visited, State &state);

        struct Node {
            Node(int _position, int _distance) {
                position = _position;
                distance = _distance;
            }
            int position;
            int distance;
        };
};

#endif // ASTAR_H_INCLUDED
