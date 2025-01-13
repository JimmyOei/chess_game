#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>

struct Move {
    int from;
    int to;
    uint8_t piece;
    bool isPromotion;
    uint8_t promotionPiece;
};

#endif