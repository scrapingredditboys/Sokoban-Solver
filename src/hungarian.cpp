#include "include/hungarian.h"

Hungarian::Hungarian(int _size) {
    size = _size;
}

int Hungarian::solve(int **graph) {
    int **originalGraph = new int*[size];
    for(int i = 0; i < size; i++) {
        originalGraph[i] = new int[size];
        for(int j = 0; j < size; j++) {
            originalGraph[i][j] = graph[i][j];
        }
    }

    if(!zeroRows(graph) || !zeroColumns(graph) || !maximizeLines(graph)) {
        for(int j = 0; j < size; j++) {
            delete[] originalGraph[j];
        }
        delete[] originalGraph;
        return -1;
    }
    int *matching = maximumMatching(graph);
    int sum = 0;

    for(int i = 0; i < size; i++) {
        if(matching[i] < 0) {
            for(int j = 0; j < size; j++) {
                delete[] originalGraph[j];
            }
            delete[] originalGraph;
            delete[] matching;
            return -1;
        }
        sum += originalGraph[matching[i]][i];
    }
    for(int j = 0; j < size; j++) {
        delete[] originalGraph[j];
    }
    delete[] originalGraph;
    delete[] matching;
    return sum;
}

bool Hungarian::zeroRows(int **graph) {
    for(int i = 0; i < size; i++) {
        int min = 9999;
        for(int j = 0; j < size; j++) {
            if(graph[i][j] < min && graph[i][j] >= 0) {
                min = graph[i][j];
            }
        }
        if(min == 9999) {
            return false;
        }
        for(int j = 0; j < size; j++) {
            graph[i][j] -= min;
        }
    }
    return true;
}

bool Hungarian::zeroColumns(int **graph) {
    for(int i = 0; i < size; i++) {
        int min = 9999;
        for(int j = 0; j < size; j++) {
            if(graph[j][i] < min && graph[j][i] >= 0) {
                min = graph[j][i];
            }
        }
        if(min == 9999) {
            return false;
        }
        for(int j = 0; j < size; j++) {
            graph[j][i] -= min;
        }
    }
    return true;
}

bool Hungarian::maximizeLines(int **graph) {
    bool assigned[size * size];
    bool assignedRows[size];
    bool assignedColumns[size];
    bool markedRows[size];
    bool markedColumns[size];

    int lines = 0;

    do {
        memset(assigned, 0, sizeof(bool) * size * size);
        memset(assignedRows, 0, sizeof(bool) * size);
        memset(assignedColumns, 0, sizeof(bool) * size);
        memset(markedRows, 0, sizeof(bool) * size);
        memset(markedColumns, 0, sizeof(bool) * size);

        int *matching = maximumMatching(graph);
        for(int i = 0; i < size; i++) {
            if(matching[i] >= 0) {
                assignedRows[matching[i]] = true;
                assignedColumns[i] = true;
                assigned[matching[i] * size + i] = true;
            }
        }
        delete[] matching;

        std::queue<int> rowsQueue;
        std::queue<int> columnsQueue;
        for(int i = 0; i < size; i++) {
            if(!assignedRows[i]) {
                rowsQueue.push(i);
                markedRows[i] = true;
            }
        }
        bool done = false;

        while(!done) {
            while(!rowsQueue.empty()) {
                int row = rowsQueue.front();
                rowsQueue.pop();

                for(int i = 0; i < size; i++) {
                    if(graph[row][i] == 0 && !markedColumns[i]) {
                        columnsQueue.push(i);
                        markedColumns[i] = true;
                    }
                }
            }
            while(!columnsQueue.empty()) {
                int column = columnsQueue.front();
                columnsQueue.pop();

                for(int i = 0; i < size; i++) {
                    if(graph[i][column] == 0 && !markedRows[i] && assigned[i * size + column]) {
                        rowsQueue.push(i);
                        markedRows[i] = true;
                    }
                }
            }
            if(rowsQueue.empty()) {
                done = true;
            }
        }

        lines = 0;
        for(int i = 0; i < size; i++) {
            if(!markedRows[i]) lines++;
            if(markedColumns[i]) lines++;
        }

        if(lines < size) {
            int min = 9999;
            for(int i = 0; i < size; i++) {
                if(markedRows[i]) {
                    for(int j = 0; j < size; j++) {
                        if(graph[i][j] < min && !markedColumns[j] && graph[i][j] >= 0) {
                            min = graph[i][j];
                        }
                    }
                }
            }
            if(min == 9999) {
                return false;
            }
            for(int i = 0; i < size; i++) {
                for(int j = 0; j < size; j++) {
                    if(markedRows[i] && !markedColumns[j]) {
                        graph[i][j] -= min;
                    }
                    else if(!markedRows[i] && markedColumns[j] && graph[i][j] >= 0) {
                        graph[i][j] += min;
                    }
                }
            }
        }
    }while(lines < size);
    return true;
}

int* Hungarian::maximumMatching(int **graph) {
    int *matchToBox = new int[size];
    memset(matchToBox, -1, sizeof(int) * size);

    for(int i = 0; i < size; i++) {
        bool seen[size];
        memset(seen, 0, sizeof(bool) * size);
        augmentPath(graph, i, seen, matchToBox);

    }
    return matchToBox;
}

bool Hungarian::augmentPath(int **graph, int box, bool *seen, int *matchToBox) {
    for(int i = 0; i < size; i++) {
        if(graph[box][i] == 0 && !seen[i]) {
            seen[i] = true;
            if(matchToBox[i] < 0 || augmentPath(graph, matchToBox[i], seen, matchToBox)) {
                matchToBox[i] = box;
                return true;
            }
        }
    }
    return false;
}
