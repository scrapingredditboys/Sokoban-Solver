#ifndef ASTAR_H_INCLUDED
#define ASTAR_H_INCLUDED

#include <ctime>
#include <functional>
#include <iostream>
#include <queue>
#include <string.h>
#include <vector>

#include "deadlockTable.h"
#include "hungarian.h"
#include "level.h"
#include "state.h"
#include "transpositionTable.h"

typedef std::function<int (const State*, const State*)> state_comparator;
typedef std::priority_queue<State*, std::vector<State*>, state_comparator> state_queue;

// In reality, this is the IDA* approach, not A*

class AStar {
    public:
        AStar(Level &_level, std::string param = "ida*");
        ~AStar();
        std::string start(double timeLimit);

    private:
        enum class Setting {
            AStar,
            IDAStar
        };

        Level &level;
        TranspositionTable table;
        int *goals;
        short **distanceToGoal;
        DeadlockTable deadlockTable;
        char *newBoard;
        char *currentBoard;
        bool *tunnels;
        bool *deadSquares;
        Setting setting;

        bool handleState(State *state, int limit, int q, state_queue &states1, state_queue &states2);
        void setCurrentBoard(State &state);
        State* getInitialState();
        void calculateH(State &state);
        int getTunnel(State &state, int box, int offset);
        bool* getPICorral(State &state, bool *visited);
        int closestGoalLowerbound(int **graph);
        int minimumCostMatchingLowerbound(int **graph);
        void initialCalculation(State &state);
        bool isLevelWon(State &state);
        void calculateDistanceToGoal(State &state);
        void calculateTunnels();
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
