#include "myLib.h"
#include "text/playerfont.h"
#include <stdlib.h>

int playerSpeed = 3;

//moves the player depending on its direction and the fixed max player speed
void movePlayer(PLAYER* player) {
    //dir == 0 -> right
    //dir == 1 -> down
    //dir == 2 -> left
    //dir == 3 -> up
    //dir == 4 -> up right
    //dir == 5 -> down right
    //dir == 6 -> up left
    //dir == 7 -> down left
    if (player->dir == 0 || player->dir == 4 || player->dir == 5) {
        player->oldx = player->x;
        player->oldy = player->y;
        player->x += playerSpeed;
    }
    if (player->dir == 1 || player->dir == 5 || player->dir == 7) {
        player->oldx = player->x;
        player->oldy = player->y;
        player->y += playerSpeed;
    }
    if (player->dir == 2 || player->dir == 6 || player->dir == 7) {
        player->oldx = player->x;
        player->oldy = player->y;
        player->x -= playerSpeed;
    }
    if (player->dir == 3 || player->dir == 4 || player->dir == 6) {
        player->oldx = player->x;
        player->oldy = player->y;
        player->y -= playerSpeed;
    }

    //wrap the player location around to the beginning if one of the player's
    //edges crosses a side of the screen
    if (player->x > 234) {
        player->x = 233;
    } else if (player->x < 6) {
        player->x = 7;
    }

    if (player->y > 154) {
        player->y = 153;
    } else if (player->y < 6) {
        player->y = 7;
    }
}

//clears the player image by drawing a background colored rectangle over its
//previous location
void clearPlayer(PLAYER player) {
    drawRect3(player.oldx - 10, player.oldy - 10, 20, 20, BG_COLOR);
}

//draws the player by iterating over a 12x12 bitmap of player images; same
//process as rendering a bitmap font. Uses a different image for each of
//the 8 possible orientations
void drawPlayer(PLAYER player) {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            if (playerfont_12x12[OFFSET(i, j, 12) + player.dir * 144]) {
                setPixel(player.x - 6 + i, player.y - 6 + j, PLAYER_COLOR);
            }
        }
    }
}

//checks whether the player collides with any boids, then sets that boid's
//isAlive to false so that it stops interacting, then increments the number of
//boids eaten
void handleBoidCollision(PLAYER player, BOID boids[]) {
    for (int i = 0; i < numBoids; i++) {
        if (boids[i].isAlive && abs(UNFIX8(boids[i].x) - player.x) < 10
            && abs(UNFIX8(boids[i].y) - player.y) < 10) {
            boids[i].isAlive = 0;
            boidsEaten += 1;
        }
    }
}
