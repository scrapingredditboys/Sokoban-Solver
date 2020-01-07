#include "include/transpositionTable.h"

TranspositionTable::TranspositionTable(Level &_level)
: level(_level) {
    table = new short[SIZE];
    memset(table, 0, sizeof(short)*SIZE);

    zobristValues = new long long*[level.getSize()];
    for(int i = 0; i < level.getSize(); i++) {
        zobristValues[i] = new long long[TYPES];
        for(int j = 0; j < TYPES; j++) {
            zobristValues[i][j] = getRandom();
        }
    }
}

TranspositionTable::~TranspositionTable() {
    for(int i = 0; i < level.getSize(); i++) {
        delete[] zobristValues[i];
    }
    delete[] zobristValues;
    delete[] table;
}

void TranspositionTable::setFinishState(int *goals) {
    long long h = 0;
    for(int i = 0; i < level.getBoxCount(); i++) {
        h ^= zobristValues[goals[i]][0];
    }
    for(int i = 0; i < level.getSize(); i++) {
        add(h ^ zobristValues[i][1], -1);
    }
}

void TranspositionTable::clear() {
    memset(table, 0, sizeof(short)*SIZE);
}

long long TranspositionTable::hashFunction(State &state) {
    long long h = 0;
    for(int i = 0; i < level.getBoxCount(); i++) {
        h ^= zobristValues[state.boxes[i]][0];
    }
    h ^= zobristValues[state.player][1];
    return h;
}

long long TranspositionTable::getRandom() {
    switch(RAND_MAX) {
        case 32767:
            return ((long long)rand() << 48) + ((long long)rand() << 32) + ((long long)rand() << 16) + (long long)rand();
        case 2147483647:
            return ((long long)rand() << 32) + (long long)rand();
    }
}
