#include "include/astar.h"

AStar::AStar(Level &_level)
: level(_level) {
    goals = new int[level.getBoxCount()];
}

AStar::~AStar() {
    delete goals;
}

void AStar::start(double timeLimit) {
    clock_t start = clock();
    int offset[4] = { -1, 1, -level.getWidth(), level.getWidth() };
    char *board = level.getBoard();
    bool *visited = new bool[level.getWidth() * level.getHeight()];

    State *initialState = getInitialState();
    calculateH(*initialState);
    int initialLimit = initialState->h;
    delete initialState;

    auto compare = [](const State *s1, const State *s2) { return s1->h < s2->h; };

    std::priority_queue<State*, std::vector<State*>, decltype(compare)> states(compare);
    int count = 0;

    for(int limit = initialLimit; ; limit++) {
        initialState = getInitialState();
        calculateH(*initialState);
        states.push(initialState);
        while(!states.empty()) {
            // Grab the state with lowest lower bound estimate
            State *state = states.top();
            states.pop();
            calculateH(*state);

            // Check if time limit is exceeded
            clock_t timeControl = clock();
            if( double(timeControl - start) / CLOCKS_PER_SEC >= timeLimit) {
                std::cout << "  Unsolved after " << double(timeControl - start) / CLOCKS_PER_SEC << "s"
                          << "   Explored nodes: " << count << std::endl;
                delete state;
                while(!states.empty()) {
                    state = states.top();
                    delete state;
                    states.pop();
                }
                delete[] visited;
                return;
            }

            count++;

            // Check if the level is complete
            bool levelWon = true;
            for(int i = 0; i < level.getBoxCount(); i++) {
                bool valid = false;
                for(int j = 0; j < level.getBoxCount(); j++) {
                    if(state->boxes[i] == goals[j]) {
                        valid = true;
                        break;
                    }
                }
                if(!valid) {
                    levelWon = false;
                    break;
                }
            }
            if(levelWon) {
                int pushes = state->g;
                delete state;
                while(!states.empty()) {
                    state = states.top();
                    delete state;
                    states.pop();
                }
                delete[] visited;
                clock_t end = clock();
                std::cout << "  Solved in " << pushes << " pushes in " << double(end - start) / CLOCKS_PER_SEC << "s"
                          << "   Explored nodes: " << count << std::endl;
                return;
            }

            // Ignore states outside current depth limit or if simple deadlock is detected
            if(state->h < 0 || state->g + state->h > limit) {
                delete state;
                continue;
            }

            // Calculate all reachable squares by player without pushing boxes
            for(int i = 0; i < level.getWidth() * level.getHeight(); i++) {
                visited[i] = false;
            }
            playerBFS(visited, *state);

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
                            int *newBoxes = new int[level.getBoxCount()];
                            memcpy(newBoxes, state->boxes, sizeof(int) * level.getBoxCount());
                            newBoxes[i] += offset[j];
                            State *newState = new State(newBoxes, newBoxes[i] - offset[j], state->g + 1);
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
    int *boxes = new int[level.getBoxCount()];
    int player = 0;

    // Run through the board, setting position values
    char *board = level.getBoard();
    int boxCount = 0;
    int goalCount = 0;
    for(int i = 0; i < level.getHeight() * level.getWidth(); i++) {
        if(board[i] == '$') {
            boxes[boxCount++] = i;
        }
        else if(board[i] == '*') {
            boxes[boxCount++] = i;
            goals[goalCount++] = i;
        }
        else if(board[i] == '.') {
            goals[goalCount++] = i;
        }
        else if(board[i] == '@' || board[i] == '+') {
            player = i;
        }
    }

    return new State(boxes, player, 0);
}

void AStar::calculateH(State &state) {
    int **graph = getBipartiteGraph(state);
    int sum = 0;
    bool deadlock = false;

    for(int i = 0; i < level.getBoxCount(); i++) {
        int min = graph[i][0];
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

    state.h = deadlock ? -1 : sum;

    for(int i = 0; i < level.getBoxCount(); i++) {
        delete[] graph[i];
    }
    delete[] graph;
}

int** AStar::getBipartiteGraph(State &state) {
    char *board = level.getBoard();
    int **graph = new int*[level.getBoxCount()];
    int width = level.getWidth();
    for(int i = 0; i < level.getBoxCount(); i++) {
        graph[i] = new int[level.getBoxCount()];
        for(int j = 0; j < level.getBoxCount(); j++) {
            graph[i][j] = -1;
        }
    }

    bool visited[level.getHeight() * width];

    // For each box, apply BFS algorithm simulating pushing boxes (not Manhattan distance) to find distance to each goal
    for(int i = 0; i < level.getBoxCount(); i++) {
        for(int j = 0; j < level.getHeight() * width; j++) {
            visited[j] = false;
        }

        std::queue<Node> nodes;
        nodes.push(Node(state.boxes[i], 0));

        while(!nodes.empty()) {
            Node node = nodes.front();
            nodes.pop();
            int p = node.position;
            int d = node.distance;

            visited[p] = true;
            if(board[p] == '.' || board[p] == '*') {
                graph[i][getGoalIndex(p)] = d;
            }

            if((board[p - 1] != '#') && (board[p + 1] != '#')) {
                if(!visited[p - 1]) {
                    nodes.push(Node(p - 1, d + 1));
                }
                if(!visited[p + 1]) {
                    nodes.push(Node(p + 1, d + 1));
                }
            }
            if((board[p - width] != '#') && (board[p + width] != '#')) {
                if(!visited[p - width]) {
                    nodes.push(Node(p - width, d + 1));
                }
                if(!visited[p + width]) {
                    nodes.push(Node(p + width, d + 1));
                }
            }
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
                }
            }
        }
    }
}
