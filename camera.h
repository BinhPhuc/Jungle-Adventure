#ifndef _CAMERA__H
#define _CAMERA__H

#include "defs.h"

struct Camera {
    int x = 0;
    int y = 0;

    void update(int playerX, int playerWidth) {
        int screenLimit = SCREEN_WIDTH * 0.7;

        if (playerX - x > screenLimit) {
            x = playerX - screenLimit;
        }

        if (x < 0) x = 0;
    }

    SDL_Rect getView() const {
        return { x, y, SCREEN_WIDTH, SCREEN_HEIGHT };
    }
};

#endif

