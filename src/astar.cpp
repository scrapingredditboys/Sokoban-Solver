#include "include/astar.h"

AStar::AStar(Level &_level, std::string param)
: level(_level),
  table(_level),
  deadlockTable(3, 3, _level.getHeight(), _level.getWidth(), level.getBoxCount(), "deadlockTable3x3.txt") {
    goals = new int[level.getBoxCount()];
    newBoard = new char[level.getSize()];
    currentBoard = new char[level.getSize()];
    tunnels = new bool[level.getSize()];
    deadSquares = new bool[level.getSize()];
    if(param == "a*") {
        setting = Setting::AStar;
    }
    else {
        setting = Setting::IDAStar;
    }
}

AStar::~AStar() {
    delete[] goals;
    for(int i = 0; i < level.getSize(); i++) {
        delete[] distanceToGoal[i];
    }
    delete[] distanceToGoal;
    delete[] newBoard;
    delete[] tunnels;
}

std::string AStar::start(double timeLimit) {
    clock_t start = clock();
    int offset[4] = { -1, 1, -level.getWidth(), level.getWidth() };
    bool *visited = new bool[level.getSize()];

    State *initialState = getInitialState();
    initialCalculation(*initialState);
    int initialLimit = initialState->h;

    table.setFinishState(goals);

    state_comparator compare = [](const State *s1, const State *s2) {
        return (s1->h * 1000 + s1->g) > (s2->h * 1000 + s2->g);
    };

    state_queue states1(compare);
    state_queue states2(compare);
    int count = 0;
    int q = 1;

    handleState(initialState, initialLimit, q, states1, states2);
    states1.push(initialState);

    for(int limit = initialLimit; ; limit += 2) {
        q = (q + 1) % 2;

        clock_t timeControl = clock();
        if(states1.empty() && states2.empty()) {
            delete[] visited;
            return "";
        }

        while((q == 0 && !states1.empty()) || (q == 1 && !states2.empty())) {
            // Grab the state with lowest lower bound estimate
            State *state;

            if(q == 0) {
                state = states1.top();
                states1.pop();
            }
            else if(q == 1) {
                state = states2.top();
                states2.pop();
            }

            // Check if time limit is exceeded
            clock_t timeControl = clock();
            if( double(timeControl - start) / CLOCKS_PER_SEC >= timeLimit) {
                delete state;
                while(!states1.empty()) {
                    state = states1.top();
                    delete state;
                    states1.pop();
                }
                while(!states2.empty()) {
                    state = states2.top();
                    delete state;
                    states2.pop();
                }
                delete[] visited;
                return "";
            }

            if(setting == Setting::AStar) {
                if(state->g > limit) {
                    limit = state->g;
                }
            }

            setCurrentBoard(*state);

            // Check if the level is complete
            if(isLevelWon(*state) && state->g > 0) {
                int pushes = state->g;
                std::string lurd = getLurd(state->history, state->g);
                delete state;
                while(!states1.empty()) {
                    state = states1.top();
                    delete state;
                    states1.pop();
                }
                while(!states2.empty()) {
                    state = states2.top();
                    delete state;
                    states2.pop();
                }
                delete[] visited;
                clock_t end = clock();
                return lurd;
            }

            memset(visited, 0, sizeof(bool) * level.getSize());
            playerBFS(visited, *state);

            bool *corral = getPICorral(*state, visited);

            // For each box, try pushing it in each direction
            for(int i = 0; i < level.getBoxCount(); i++) {
                if(corral != nullptr && corral[i] == false) {
                    continue;
                }

                int box = state->boxes[i];
                for(int j = 0; j < 4; j++) {
                    if(!visited[box - offset[j]] || deadSquares[box + offset[j]] || newBoard[box + offset[j]] == '#') {
                        continue;
                    }

                    bool valid = true;
                    for(int k = 0; k < level.getBoxCount(); k++) {
                        if(state->boxes[k] == box + offset[j]) {
                            valid = false;
                            break;
                        }
                    }
                    if(!valid) {
                        continue;
                    }

                    int moveCount = getTunnel(*state, box, offset[j]);

                    short *newBoxes = new short[level.getBoxCount()];
                    memcpy(newBoxes, state->boxes, sizeof(short) * level.getBoxCount());
                    newBoxes[i] += offset[j] * moveCount;

                    int *newHistory = new int[state->g + moveCount];
                    memcpy(newHistory, state->history, sizeof(int) * state->g);
                    for(int k = 0; k < moveCount; k++) {
                        newHistory[state->g + k] = (box + offset[j] * (k - 1)) + level.getSize() * j;
                    }

                    State *newState = new State(newBoxes, newBoxes[i] - offset[j], state->g + moveCount, newHistory, newBoxes[i]);
                    count++;
                    if(handleState(newState, limit, q, states1, states2)) {
                        if(q == 0) {
                            states1.push(newState);
                        }
                        else if(q == 1) {
                            states2.push(newState);
                        }
                    }
                }
            }

            if(corral != nullptr) {
                delete[] corral;
            }
            delete state;
        }
    }
    return "";
}

bool AStar::handleState(State *state, int limit, int q, state_queue &states1, state_queue &states2) {
    // Calculate all reachable squares by player without pushing boxes
    bool *visited = new bool[level.getSize()];
    memset(visited, 0, sizeof(bool) * level.getSize());

    setCurrentBoard(*state);
    playerBFS(visited, *state);
    delete[] visited;

    // Transposition table
    short value = table.get(*state);
    if(value > 0) {
        if(state->g < value) {
            table.add(*state);
        }
        else {
            delete state;
            return false;
        }
    }
    else if(value == 0) {
        table.add(*state);
    }

    calculateH(*state);

    // Ignore states outside current depth limit or if deadlock is detected
    if(state->h < 0 || deadlockTable.isDeadlock(newBoard, *state, state->lastMove)) {
        delete state;
        return false;
    }

    if(setting == Setting::IDAStar) {
        if(state->g + state->h > limit) {
            if(q == 0) {
                states2.push(state);
            }
            else if(q == 1) {
                states1.push(state);
            }
            return false;
        }
    }
    return true;
}

void AStar::setCurrentBoard(State &state) {
    memcpy(currentBoard, newBoard, level.getSize());
    for(int i = 0; i < level.getBoxCount(); i++) {
        currentBoard[state.boxes[i]] = '$';
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

    delete[] newBoard;
    newBoard = new char[level.getSize()];
    memcpy(newBoard, board, level.getSize());

    for(short i = 0; i < level.getSize(); i++) {
        if(board[i] == '$') {
            boxes[boxCount++] = i;
            newBoard[i] = ' ';
        }
        else if(board[i] == '*') {
            boxes[boxCount++] = i;
            goals[goalCount++] = i;
            newBoard[i] = '.';
        }
        else if(board[i] == '+') {
            goals[goalCount++] = i;
            player = i;
            newBoard[i] = '.';
        }
        else if(board[i] == '.') {
            goals[goalCount++] = i;
        }
        else if(board[i] == '@') {
            player = i;
            newBoard[i] = ' ';
        }
    }

    return new State(boxes, player, 0, nullptr, -1);
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

int AStar::getTunnel(State &state, int box, int offset) {
    int moveCount = 1;
    if(tunnels[box] && tunnels[box + offset]) {
        int tunnel = box + 2 * offset;
        while(tunnels[tunnel] && currentBoard[tunnel] != '$') {
            moveCount++;
            tunnel += offset;
        }
        if(currentBoard[tunnel] != '$' && newBoard[tunnel] != '#') {
            moveCount++;
        }
    }
    return moveCount;
}

bool* AStar::getPICorral(State &state, bool *visited) {
    int visitedCorral[level.getSize()];
    memset(visitedCorral, 0, sizeof(int) * level.getSize());
    int corralCounter = 0;
    int width = level.getWidth();

    for(int i = level.getWidth() + 1; i < level.getSize() - level.getWidth() - 1; i++) {
        if(visited[i] == true || visitedCorral[i] != 0 || (currentBoard[i] != ' ' && currentBoard[i] != '.')) {
            continue;
        }
        corralCounter++;
        bool relevantBoxes[level.getBoxCount()];
        bool hasGoal = false;
        memset(relevantBoxes, 0, sizeof(bool) * level.getBoxCount());

        std::queue<int> positions;
        positions.push(i);

        int offset[4] = { -1, 1, -width, width };

        while(!positions.empty()) {
            int p = positions.front();
            positions.pop();

            if(p < width + 1 || p >= level.getSize() - width - 1 || (p % width == 0) || (p % width == width - 1)) {
                continue;
            }

            if(currentBoard[p] != '$') {
                visitedCorral[p] = corralCounter;
                if(currentBoard[p] == '.') {
                    hasGoal = true;
                }
            }
            else {
                relevantBoxes[getBoxIndex(state, p)] = true;
                continue;
            }

            for(int i = 0; i < 4; i++) {
                int to = p + offset[i];
                if(visitedCorral[to] != corralCounter && currentBoard[to] != '#') {
                    positions.push(to);
                }
            }
        }

        if(!hasGoal) {
            bool allBoxesOnGoals = true;
            for(int i = 0; i < level.getBoxCount(); i++) {
                if(relevantBoxes[i] && newBoard[state.boxes[i]] != '.') {
                    allBoxesOnGoals = false;
                    break;
                }
            }
            if(allBoxesOnGoals) {
                continue;
            }
        }

        bool isPICorral = false;
        for(int i = 0; i < level.getBoxCount(); i++) {
            if(relevantBoxes[i] == false) {
                continue;
            }
            isPICorral = true;

            int box = state.boxes[i];
            bool valid = true;
            for(int j = 0; j < 4; j++) {
                int to = box + offset[j];
                int from = box - offset[j];

                if(visitedCorral[to] == corralCounter) {
                    int boxIndex = getBoxIndex(state, from);
                    if(visitedCorral[from] != corralCounter && currentBoard[from] != '#' &&
                       (boxIndex != -1 || relevantBoxes[boxIndex] == false) && visited[from] == false) {
                        valid = false;
                        break;
                    }
                }
                else if(currentBoard[to] != '#' && currentBoard[to] != '$' &&
                        currentBoard[from] != '#' && currentBoard[from] != '$' &&
                        visitedCorral[from] != corralCounter){
                    valid = false;
                    break;
                }
            }

            if(!valid) {
                isPICorral = false;
                break;
            }
        }

        if(isPICorral) {
            bool *corralBoxes = new bool[level.getBoxCount()];
            memcpy(corralBoxes, relevantBoxes, sizeof(bool) * level.getBoxCount());

            return corralBoxes;
        }
    }

    return nullptr;
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
    calculateTunnels();
    int **graph = getBipartiteGraph(state);

    state.h = minimumCostMatchingLowerbound(graph);

    for(int i = 0; i < level.getBoxCount(); i++) {
        delete[] graph[i];
    }
    delete[] graph;
}

bool AStar::isLevelWon(State &state) {
    if(table.get(state) != -1) {
        return false;
    }
    for(int i = level.getWidth() + 1; i < level.getSize() - level.getWidth() - 1; i++) {
        if(newBoard[i] == '.' && currentBoard[i] != '$') {
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

            if(visited[p]) {
                continue;
            }

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

    for(int i = 0; i < level.getSize(); i++) {
        bool dead = true;
        for(int j = 0; j < level.getBoxCount(); j++) {
            if(distanceToGoal[i][j] != -1) {
                dead = false;
                break;
            }
        }
        deadSquares[i] = dead;
    }
}

void AStar::calculateTunnels() {
    char *board = level.getBoard();
    for(int i = 0; i < level.getSize(); i++) {
        if(i < level.getWidth() || i >= (level.getSize() - level.getWidth()) ||
          (i % level.getWidth() == 0) || (i % level.getWidth() == level.getWidth() - 1)) {
            tunnels[i] = false;
        }
        else if(((board[i - 1] == '#' && board[i + 1] == '#') ||
                 (board[i - level.getWidth()] == '#' && board[i + level.getWidth()] == '#')) &&
                 (board[i] == ' ' || board[i] == '$' || board[i] == '@')) {
            tunnels[i] = true;
        }
        else {
            tunnels[i] = false;
        }
    }
}

int** AStar::getBipartiteGraph(State &state) {
    int **graph = new int*[level.getBoxCount()];
    for(int i = 0; i < level.getBoxCount(); i++) {
        graph[i] = new int[level.getBoxCount()];
        memset(graph[i], -1, sizeof(int) * level.getBoxCount());
    }
    for(int i = 0; i < level.getBoxCount(); i++) {
        for(int j = 0; j < level.getBoxCount(); j++) {
            graph[i][j] = distanceToGoal[state.boxes[i]][j];
        }
    }
    return graph;
}

int AStar::getGoalIndex(int position) {
    if(newBoard[position] != '.') {
        return -1;
    }
    for(int i = 0; i < level.getBoxCount(); i++) {
        if(goals[i] == position) {
            return i;
        }
    }
    return -1;
}

int AStar::getBoxIndex(State &state, short position) {
    if(currentBoard[position] != '$') {
        return -1;
    }
    for(int i = 0; i < level.getBoxCount(); i++) {
        if(state.boxes[i] == position) {
            return i;
        }
    }
    return -1;
}

void AStar::playerBFS(bool *visited, State &state) {
    std::queue<int> positions;
    positions.push(state.player);

    int offset[4] = { -1, 1, -level.getWidth(), level.getWidth() };

    while(!positions.empty()) {
        int p = positions.front();
        positions.pop();

        visited[p] = true;

        for(int i = 0; i < 4; i++) {
            int to = p + offset[i];
            if(!visited[to] && newBoard[to] != '#' && currentBoard[to] != '$') {
                positions.push(to);
                if(to < state.player) {
                    state.player = to;
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
        setCurrentBoard(*state);
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

//Move inertia showed to be beneficial in some levels, but a burden in many others.
