#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <iostream>

class State {
    public:
        State(short *_boxes, short _player, short _g, int *_history, int _lastMove)
        : boxes(_boxes),
          player(_player),
          g(_g),
          history(_history),
          lastMove(_lastMove) {
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
        int lastMove;
};

#endif // STATE_H_INCLUDED
