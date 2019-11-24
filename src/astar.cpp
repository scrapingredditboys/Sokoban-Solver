#include "include/astar.h"

AStar::AStar(Level &_level)
: level(_level),
  table(_level) {
    goals = new int[level.getBoxCount()];
}

AStar::~AStar() {
    delete[] goals;
    for(int i = 0; i < level.getSize(); i++) {
        delete[] distanceToGoal[i];
    }
    delete[] distanceToGoal;
}

void AStar::start(double timeLimit) {
    clock_t start = clock();
    int offset[4] = { -1, 1, -level.getWidth(), level.getWidth() };
    char *board = level.getBoard();
    bool *visited = new bool[level.getSize()];

    State *initialState = getInitialState();
    initialCalculation(*initialState);
    int initialLimit = initialState->h;
    delete initialState;

    auto compare = [](const State *s1, const State *s2) { return s1->h < s2->h; };

    std::priority_queue<State*, std::vector<State*>, decltype(compare)> states(compare);
    int count = 0;

    for(int limit = initialLimit;  ; limit += 2) {
        table.clear();
        initialState = getInitialState();
        calculateH(*initialState);
        states.push(initialState);
        while(!states.empty()) {
            // Grab the state with lowest lower bound estimate
            State *state = states.top();
            states.pop();
            count++;

            // Check if time limit is exceeded
            clock_t timeControl = clock();
            if( double(timeControl - start) / CLOCKS_PER_SEC >= timeLimit) {
                /*std::cout << "  Unsolved after " << double(timeControl - start) / CLOCKS_PER_SEC << "s"
                          << "   Explored nodes: " << count << std::endl;*/
                delete state;
                while(!states.empty()) {
                    state = states.top();
                    delete state;
                    states.pop();
                }
                delete[] visited;
                return;
            }

            // Check if the level is complete
            if(isLevelWon(*state)) {
                int pushes = state->g;
                std::cout << getLurd(state->history, state->g);
                delete state;
                while(!states.empty()) {
                    state = states.top();
                    delete state;
                    states.pop();
                }
                delete[] visited;
                clock_t end = clock();
                /*std::cout << "  Solved in " << pushes << " pushes in " << double(end - start) / CLOCKS_PER_SEC << "s"
                          << "   Explored nodes: " << count << std::endl;*/
                return;
            }

            // Calculate all reachable squares by player without pushing boxes
            for(int i = 0; i < level.getSize(); i++) {
                visited[i] = false;
            }
            playerBFS(visited, *state);

            // Transposition table
            short value = table.get(*state);
            if(value > 0) {
                if(state->g < value) {
                    table.add(*state);
                }
                else {
                    delete state;
                    continue;
                }
            }
            else {
                table.add(*state);
            }

            calculateH(*state);

            // Ignore states outside current depth limit or if simple deadlock is detected
            if(state->h < 0 || state->g + state->h > limit) {
                delete state;
                continue;
            }

            // For each box, try pushing it in each direction
            for(int i = 0; i < level.getBoxCount(); i++) {
                int box = state->boxes[i];
                for(int j = 0; j < 4; j++) {
                    if(visited[box - offset[j]] && board[box + offset[j]] != '#') {
                        bool valid = true;
                        for(int k = 0; k < level.getBoxCount(); k++) {
                            if(state->boxes[k] == box + offset[j]) {
                                valid = false;
                                break;
                            }
                        }
                        if(valid) {
                            short *newBoxes = new short[level.getBoxCount()];
                            memcpy(newBoxes, state->boxes, sizeof(short) * level.getBoxCount());
                            newBoxes[i] += offset[j];

                            int *newHistory = new int[state->g + 1];
                            memcpy(newHistory, state->history, sizeof(int) * state->g);
                            newHistory[state->g] = (box - offset[j]) + level.getSize() * j;

                            State *newState = new State(newBoxes, newBoxes[i] - offset[j], state->g + 1, newHistory);
                            states.push(newState);
                        }
                    }
                }
            }
            delete state;
        }
    }
}

State* AStar::getInitialState() {
    // Get box and goal positions in initial configuration, as well as player position
    short *boxes = new short[level.getBoxCount()];
    short player = 0;

    // Run through the board, setting position values
    char *board = level.getBoard();
    int boxCount = 0;
    int goalCount = 0;
    for(short i = 0; i < level.getSize(); i++) {
        if(board[i] == '$') {
            boxes[boxCount++] = i;
        }
        else if(board[i] == '*') {
            boxes[boxCount++] = i;
            goals[goalCount++] = i;
        }
        else if(board[i] == '+') {
            goals[goalCount++] = i;
            player = i;
        }
        else if(board[i] == '.') {
            goals[goalCount++] = i;
        }
        else if(board[i] == '@') {
            player = i;
        }
    }

    return new State(boxes, player, 0, nullptr);
}

void AStar::calculateH(State &state) {
    int **graph = getBipartiteGraph(state);

    //state.h = closestGoalLowerbound(graph);
    state.h = minimumCostMatchingLowerbound(graph);

    for(int i = 0; i < level.getBoxCount(); i++) {
        delete[] graph[i];
    }
    delete[] graph;
}

int AStar::closestGoalLowerbound(int **graph) {
    int sum = 0;
    bool deadlock = false;

    for(int i = 0; i < level.getBoxCount(); i++) {
        int min = 9999;
        int max = -1;
        for(int j = 0; j < level.getBoxCount(); j++) {
            if(graph[i][j] < min && graph[i][j] >= 0) {
                min = graph[i][j];
            }
            if(graph[i][j] > max) {
                max = graph[i][j];
            }
        }
        if(max == -1) {
            deadlock = true;
            break;
        }
        sum += min;
    }

    return deadlock ? -1 : sum;
}

int AStar::minimumCostMatchingLowerbound(int **graph) {
    Hungarian h(level.getBoxCount());
    return h.solve(graph);
}

void AStar::initialCalculation(State &state) {
    calculateDistanceToGoal(state);
    int **graph = getBipartiteGraph(state);

    state.h = minimumCostMatchingLowerbound(graph);

    for(int i = 0; i < level.getBoxCount(); i++) {
        delete[] graph[i];
    }
    delete[] graph;
}

bool AStar::isLevelWon(State &state) {
    for(int i = 0; i < level.getBoxCount(); i++) {
        bool valid = false;
        for(int j = 0; j < level.getBoxCount(); j++) {
            if(state.boxes[i] == goals[j]) {
                valid = true;
                break;
            }
        }
        if(!valid) {
            return false;
        }
    }
    return true;
}

void AStar::calculateDistanceToGoal(State &state) {
    char *board = level.getBoard();
    int width = level.getWidth();
    distanceToGoal = new short*[level.getSize()];
    bool visited[level.getSize()];

    for(int i = 0; i < level.getSize(); i++) {
        distanceToGoal[i] = new short[level.getBoxCount()];

        for(int j = 0; j < level.getSize(); j++) {
            visited[j] = false;
        }
        for(int j = 0; j < level.getBoxCount(); j++) {
            distanceToGoal[i][j] = -1;
        }

        std::queue<DistanceNode> nodes;
        nodes.push(DistanceNode(i, 0));

        while(!nodes.empty()) {
            DistanceNode node = nodes.front();
            nodes.pop();
            int p = node.position;
            int d = node.distance;

            visited[p] = true;
            if(board[p] == '.' || board[p] == '*' || board[p] == '+') {
                distanceToGoal[i][getGoalIndex(p)] = d;
            }

            if((p % width != 0) && (p % width != width - 1)) {
                if((board[p - 1] != '#') && (board[p + 1] != '#')) {
                    if(!visited[p - 1]) {
                        nodes.push(DistanceNode(p - 1, d + 1));
                    }
                    if(!visited[p + 1]) {
                        nodes.push(DistanceNode(p + 1, d + 1));
                    }
                }
            }
            if((p >= width) && (p < level.getSize() - width)) {
                if((board[p - width] != '#') && (board[p + width] != '#')) {
                    if(!visited[p - width]) {
                        nodes.push(DistanceNode(p - width, d + 1));
                    }
                    if(!visited[p + width]) {
                        nodes.push(DistanceNode(p + width, d + 1));
                    }
                }
            }
        }
    }
}

int** AStar::getBipartiteGraph(State &state) {
    int **graph = new int*[level.getBoxCount()];
    int width = level.getWidth();
    for(int i = 0; i < level.getBoxCount(); i++) {
        graph[i] = new int[level.getBoxCount()];
        for(int j = 0; j < level.getBoxCount(); j++) {
            graph[i][j] = -1;
        }
    }
    for(int i = 0; i < level.getBoxCount(); i++) {
        for(int j = 0; j < level.getBoxCount(); j++) {
            graph[i][j] = distanceToGoal[state.boxes[i]][j];
        }
    }
    return graph;
}

int AStar::getGoalIndex(int position) {
    for(int i = 0; i < level.getBoxCount(); i++) {
        if(goals[i] == position) {
            return i;
        }
    }
    return -1;
}

int AStar::getBoxIndex(State &state, short position) {
    for(int i = 0; i < level.getBoxCount(); i++) {
        if(state.boxes[i] == position) {
            return i;
        }
    }
    return -1;
}

void AStar::playerBFS(bool *visited, State &state) {
    char *board = level.getBoard();
    std::queue<int> positions;
    positions.push(state.player);

    int offset[4] = { -1, 1, -level.getWidth(), level.getWidth() };

    while(!positions.empty()) {
        int p = positions.front();
        positions.pop();

        visited[p] = true;

        for(int i = 0; i < 4; i++) {
            if(!visited[p + offset[i]] && board[p + offset[i]] != '#') {
                bool valid = true;
                for(int j = 0; j < level.getBoxCount(); j++) {
                    if(state.boxes[j] == p + offset[i]) {
                        valid = false;
                        break;
                    }
                }
                if(valid) {
                    positions.push(p + offset[i]);
                    if(p + offset[i] < state.player) {
                        state.player = p + offset[i];
                    }
                }
            }
        }
    }
}

std::string AStar::getLurd(int *history, int n) {
    std::string lurd = "";
    State *state = getInitialState();
    int offset[4] = { -1, 1, -level.getWidth(), level.getWidth() };
    char LURD[] = "LRUD";

    for(int i = 0; i < n; i++) {
        int j = 0;
        int pos = history[i];
        while(pos >= level.getSize()) {
            pos -= level.getSize();
            j++;
        }
        lurd += getPath(*state, pos);
        lurd += LURD[j];
        state->boxes[getBoxIndex(*state, pos + offset[j])] += offset[j];
        state->player = pos + offset[j];
    }

    delete state;
    return lurd;
}

std::string AStar::getPath(State &state, int to) {
    char *board = level.getBoard();
    std::queue<PathNode> positions;
    positions.push(PathNode(state.player, ""));

    int offset[4] = { -1, 1, -level.getWidth(), level.getWidth() };
    char LURD[] = "lrud";

    bool visited[level.getSize()];
    memset(visited, 0, sizeof(bool) * level.getSize());

    while(!positions.empty()) {
        PathNode p = positions.front();
        positions.pop();

        if(visited[p.position]) {
            continue;
        }

        visited[p.position] = true;

        if(p.position == to) {
            return p.path;
        }

        for(int i = 0; i < 4; i++) {
            if(board[p.position + offset[i]] != '#') {
                bool valid = true;
                for(int j = 0; j < level.getBoxCount(); j++) {
                    if(state.boxes[j] == p.position + offset[i]) {
                        valid = false;
                        break;
                    }
                }
                if(valid) {
                    positions.push(PathNode(p.position + offset[i], p.path + LURD[i]));
                }
            }
        }
    }
}

// Failed ideas

// The cost of computing this function almost always outweighs its benefits
bool AStar::isDirectBoxDeadlock(State &state, bool *visited) {
    char *board = level.getBoard();
    int offset[4] = { -1, 1, -level.getWidth(), level.getWidth() };
    bool hasMoves[level.getBoxCount()];
    memset(&hasMoves, 0, sizeof(bool) * level.getBoxCount());

    for(int i = 0; i < level.getBoxCount(); i++) {
        int box = state.boxes[i];
        for(int j = 0; j < 4; j++) {
            if(visited[box - offset[j]] && board[box + offset[j]] != '#') {
                bool valid = true;
                for(int k = 0; k < level.getBoxCount(); k++) {
                    if(state.boxes[k] == box + offset[j] || state.boxes[k] == box - offset[j]) {
                        valid = false;
                        break;
                    }
                }
                if(valid) {
                    hasMoves[i] = true;
                    break;
                }
            }
        }
    }

    for(int i = 0; i < level.getBoxCount(); i++) {
        if(hasMoves[i] || getGoalIndex(state.boxes[i]) != -1) {
            continue;
        }

        bool deadlock = true;
        std::queue<short> boxes;
        boxes.push(state.boxes[i]);
        bool explored[level.getSize()];
        memset(&explored, 0, sizeof(bool) * level.getSize());

        while(!boxes.empty()) {
            short position = boxes.front();
            boxes.pop();
            short box = getBoxIndex(state, position);
            explored[position] = true;
            if(box != -1 && hasMoves[getBoxIndex(state, position)]) {
                deadlock = false;
                break;
            }

            for(int j  = 0; j < 4; j++) {
                short index = position + offset[j];
                if(!explored[index] && !visited[index] && board[index] != '#') {
                    boxes.push(index);
                }
            }
        }

        if(deadlock) {
            return true;
        }
    }
    return false;
}
