#ifndef TRANSPOSITIONTABLE_H_INCLUDED
#define TRANSPOSITIONTABLE_H_INCLUDED

#define SIZE (1*1024*1024) // 2 MB
#define TYPES 2

#include <bitset>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "level.h"
#include "state.h"

class TranspositionTable {
    public:
        TranspositionTable(Level &_level);
        ~TranspositionTable();
        void add(State &state) { add(hashFunction(state), state.g); }
        short get(State &state) { return get(hashFunction(state)); }
        void clear();
        long long getRandom();
        long long hashFunction(State &state);
        short *table;

    private:
        Level &level;
        long long **zobristValues;


        void add(long long hash, short value) { table[hash % SIZE] = value; }
        short get(long long hash) { return table[hash % SIZE]; }


};

#endif // TRANSPOSITIONTABLE_H_INCLUDED
