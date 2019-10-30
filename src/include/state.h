#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <iostream>

class State {
    public:
        State(int *_boxes, int _player, int _g)
        : boxes(_boxes),
          player(_player),
          g(_g) {
              h = 0;
        }
        ~State() {
            delete[] boxes;
        }

        int *boxes;
        int player;
        int h;
        int g;
};

#endif // STATE_H_INCLUDED
