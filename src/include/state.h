#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <iostream>

class State {
    public:
        State(short *_boxes, short _player, short _g, int *_history)
        : boxes(_boxes),
          player(_player),
          g(_g),
          history(_history) {
              h = 0;
        }
        ~State() {
            delete[] boxes;
            if(history != nullptr) {
                delete[] history;
            }
        }

        short *boxes;
        short player;
        short h;
        short g;
        int *history;
};

#endif // STATE_H_INCLUDED
