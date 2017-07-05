//feed the boids, tuppence a bag
#include "myLib.h"
#include "foodimage.h"
#include <stdlib.h>

//clears the food from its previous location, then moves it elswehere on the
//screen such that its edges do not cross over the sides of the screen
void moveFood(FOOD* food) {
    drawRect3(food->x - 4, food->y - 4, 8, 8, BG_COLOR);
    food->x = 10 + rand() % 220;
    food->y = 10 + rand() % 140;
}

//draws the food as an image with DMA
void drawFood(FOOD food) {
    drawImage3(food.x - 4, food.y - 4, 8, 8, (u16*) &foodimage);
}

//checks if the food gets hit by a player or a boid, then increments the player
//or boid score depending on the collision
void handleFoodCollision(PLAYER player, BOID boids[], FOOD* food) {
    //if the player gets close enough to the food, moves the food, then
    //respawns all of the birds
    if (abs(player.x - food->x) < 11 && abs(player.y - food->y) < 11) {
        moveFood(food);
        for (int i = 0; i < numBoids; i++) {
            if (!boids[i].isAlive) {
                boids[i].isAlive = 1;
            }
        }
        playerScore += 1;
    } else { //if a bird eats the food, move the food but don't respawn boids
        for (int i = 0; i < numBoids; i++) {
            if (abs(UNFIX8(boids[i].x) - food->x) < 6
                && abs(UNFIX8(boids[i].y) - food->y) < 6) {
                moveFood(food);
                boidScore += 1;
            }
        }
    }
}
