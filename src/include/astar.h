#ifndef ASTAR_H_INCLUDED
#define ASTAR_H_INCLUDED

#include <ctime>
#include <iostream>
#include <queue>
#include <string.h>
#include <vector>

#include "hungarian.h"
#include "level.h"
#include "state.h"
#include "transpositionTable.h"

// In reality, this is the IDA* approach, not A*

class AStar {
    public:
        AStar(Level &_level);
        ~AStar();
        std::string start(double timeLimit);

    private:
        Level &level;
        TranspositionTable table;
        int *goals;
        short **distanceToGoal;

        State* getInitialState();
        void calculateH(State &state);
        int closestGoalLowerbound(int **graph);
        int minimumCostMatchingLowerbound(int **graph);
        void initialCalculation(State &state);
        bool isLevelWon(State &state);
        void calculateDistanceToGoal(State &state);
        int** getBipartiteGraph(State &state);
        int getGoalIndex(int position);
        int getBoxIndex(State &state, short position);
        void playerBFS(bool *visited, State &state);
        std::string getLurd(int *history, int n);
        std::string getPath(State &state, int to);

        struct DistanceNode {
            DistanceNode(int _position, int _distance) {
                position = _position;
                distance = _distance;
            }
            int position;
            int distance;
        };

        struct PathNode {
            PathNode(int _position, std::string _path) {
                position = _position;
                path = _path;
            }
            int position;
            std::string path;
        };

        // Failed ideas
        bool isDirectBoxDeadlock(State &state, bool *visited);
};

#endif // ASTAR_H_INCLUDED
