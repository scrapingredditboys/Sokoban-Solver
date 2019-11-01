#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <iostream>

class State {
    public:
        State(short *_boxes, short _player, short _g)
        : boxes(_boxes),
          player(_player),
          g(_g) {
              h = 0;
        }
        ~State() {
            delete[] boxes;
        }

        short *boxes;
        short player;
        short h;
        short g;
};

#endif // STATE_H_INCLUDED
