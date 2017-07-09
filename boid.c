//file defining the boid structure
#include <stdio.h>
#include "text/text.h"
#include "myLib.h"
#include <stdlib.h>

int maxSpeed = FIX8(3);
int numBoids = 30;
int cohesion = FIX8(1) >> 4;
int sync = FIX8(1) >> 2;
int repulsion = FIX8(1) >> 3;
int viewRadius = FIX8(50);

//NOTE: FIX8 and UNFIX8 convert an int value to and from a fixed point decimal
//value. This shows up when converting decimal boid coordinates and velocities
//to integer values for rendering

//cohesion rule; move the boid toward the center of mass of all of its neighbors
void rule1(BOID* cur, BOID* others, int* neighbors) {
    //temporary acceleration vector to be added to cur's velocity
    int ax = 0;
    int ay = 0;

    //count of 1s in the neighbors array; used to calculate the center of mass
    //includes the current boid
    int neighborCount = 1;

    //for each neighboring boid in others, add that boid's x and y positions to
    //the current acceleration vector and increment the neighbor count
    for (int i = 0; i < numBoids; i++) {
        if (neighbors[i] == 1) {
            ax += others[i].x;
            ay += others[i].y;
            neighborCount += 1;
        }
    }

    //adds the current boid's location to the center of mass calculation
    ax += cur->x;
    ay += cur->y;

    //divide the summed locations of neighbors by the number of neighbors
    ax /= neighborCount;
    ay /= neighborCount;

    //subtract cur's location from the calculated center of mass to get a vector
    //relative to the origin, rather than cur's location
    ax -= cur->x;
    ay -= cur->y;

    //reduce the acceleration vector by a defined cohesion constant
    ax *= cohesion;
    ay *= cohesion;

    //add the acceleration vector to the boid's velocity
    cur->vx += UNFIX8(ax);
    cur->vy += UNFIX8(ay);
}

//repulsion rule; attempt to move the boid to a safe distance from its neighbors
void rule2(BOID* cur, BOID* others, int* neighbors) {
    //acceleration vecter
    int ax = 0;
    int ay = 0;

    //vector for temporary calculations
    int tx = 0;
    int ty = 0;

    //for each neighboring boid in others, subtract that boid's x and y position
    //from the temp acceleration vector, pseudo normalize that vector, divide it
    //by the repulsion constant, then add it to the total repulsion vector
    for (int i = 0; i < numBoids; i++) {
        //if others[i] is a neighbor of cur, subtract its position from cur's
        //position, yielding a vector facing away from others[i]
        if (neighbors[i] == 1 && &others[i] != cur) {
            tx = others[i].x - cur->x;
            ty = others[i].y - cur->y;
        }

        //normalize x to be <= 1
        if (tx > FIX8(1)) {
            tx = FIX8(1);
        } else if (tx < FIX8(-1)) {
            tx = FIX8(-1);
        }

        //normalize
        if (ty > FIX8(1)) {
            ty = FIX8(1);
        } else if (ty < FIX8(-1)) {
            ty = FIX8(-1);
        }

        //scale the temporary repulsion vector by repulsion
        tx *= repulsion;
        ty *= repulsion;

        //add the temp vector to the total acceleration vector
        ax -= UNFIX8(tx);
        ay -= UNFIX8(ty);
    }

    //add the acceleration vector to the current velocity
    cur->vx += ax;
    cur->vy += ay;
}

//sync rule; attempt to make a boid's velocity similar to those of its neighbors
void rule3(BOID* cur, BOID* others, int* neighbors) {
    //temporary acceleration vector
    int ax = 0;
    int ay = 0;

    //count of neighbors used to normalize calculated sync value
    int neighborCount = 0;

    //for each of the neighboring boids, adds the neighbor's velocity to the
    //acceleration vector to determine an aggregate boid heading
    for (int i = 0; i < numBoids; i++) {
        if (neighbors[i] == 1 && &others[i] != cur) {
            ax += others[i].vx;
            ay += others[i].vy;

            neighborCount += 1;
        }
    }

    //add the current boid's velocity to the aggregate heading for consistency
    ax += cur->vx;
    ay += cur->vy;

    //divide the heading by the number of neighbors to obtain an average heading
    ax /= neighborCount;
    ay /= neighborCount;

    //scale the heading by a constant synchronization value
    ax *= sync;
    ay *= sync;

    //add the acceleration vector to the current boid's velocity
    cur->vx += UNFIX8(ax);
    cur->vy += UNFIX8(ay);
}

//find food rule; make the boid move toward the food if the food is withing 4
//times the viewing radius
void rule4(BOID* cur, FOOD* food) {
    if (SQR(cur->x - FIX8(food->x)) + SQR(cur->y - FIX8(food->y)) < SQR(viewRadius)) {
        cur->vx += UNFIX8((FIX8(food->x) - cur->x) * (cohesion << 2));
        cur->vy += UNFIX8((FIX8(food->y) - cur->y) * (cohesion << 2));
    }
}

//avoid player rule; make the boid move away from the player if the player is
//within the view radius
void rule5(BOID* cur, PLAYER* player) {
    if (SQR(cur->x - FIX8(player->x)) + SQR(cur->y - FIX8(player->y)) < SQR(viewRadius)) {
        cur->vx -= UNFIX8((FIX8(player->x) - cur->x) * (repulsion >> 2));
        cur->vy -= UNFIX8((FIX8(player->y) - cur->y) * (repulsion >> 2));
    }
}

//calculates the new location of each living boid based on the results of each
//of the boid rules, then updates all of their positions simultaneously
void moveAllBoids(BOID* boids, PLAYER* player, FOOD* food) {
    //calculates new locations for all living boids
    int neighbors[numBoids * numBoids];
    for (int i = 0; i < numBoids; i++) {
        for (int j = 0; j < numBoids; j++) {
            if (SQR_DIST(boids + i, boids + j) < SQR(viewRadius) && i != j) {
                neighbors[numBoids * i + j] = 1;
            } else {
                neighbors[0] = 0;
            }
        }
    }
    for (int i = 0; i < numBoids; i++) {
        if (boids[i].isAlive) {
            //apply the cohesion, repulsion, synchronization, food attraction, and
            //player repulsion rules to the current boid
            BOID* cur = boids + i;
            rule1(cur, boids, neighbors + i * numBoids);
            rule2(cur, boids, neighbors + i * numBoids);
            rule3(cur, boids, neighbors + i * numBoids);
            rule4(cur, food);
            rule5(cur, player);
        }
    }

    //updates each boid's previous position with its current position, then
    //updates its position to the position calculated in the previous loop
    for (int i = 0; i < numBoids; i++) {
        if (boids[i].isAlive) {
            moveBoid(boids + i);
        }
    }
}

//Clears all of the boids that are currently alive
void clearAllBoids(BOID* boids) {
    static BOID* cur;
    for (int i = 0; i < numBoids; i++) {
        cur = boids + i;
        drawRect3((UNFIX8(cur->oldx) - 5), (UNFIX8(cur->oldy) - 5), 10, 10, BG_COLOR);
    }
}

//Draws all of the boids to the screen that are currently alive
void drawAllBoids(BOID* boids) {
    static BOID* cur;
    static int x;
    static int y;
    for (int i = 0; i < numBoids; i++) {
        if (boids[i].isAlive) {
            cur = boids + i;
            x = UNFIX8(cur->x);
            y = UNFIX8(cur->y);
            drawRect3(x - 2, y - 2, 4, 4, cur->color);
            drawRect3(x + UNFIX8(cur->vx) - 1,
                    y + UNFIX8(cur->vy) - 1, 3, 3, cur->color);
        }
    }
}

//moves a boid's position by its x and y velocities
void moveBoid(BOID* cur) {
    cur->oldx = cur->x;
    cur->oldy = cur->y;
    //limits a boid's velocity to the maximum velocity value
    int squareSpeed = SQR(cur->vx) + SQR(cur->vy);

    if (squareSpeed > SQR(maxSpeed)) {
        //These next four lines approximately compute the square root of 
        //the squared speed with extremely high accuracy
        int unfixSquareSpeed = squareSpeed >> 16;
        int guess = 100;
        for (int i = 0; i < 5; i++) {
            guess -= (SQR(guess) - unfixSquareSpeed) / (guess << 1);
        }
      
        cur->vy = cur->vy / guess * UNFIX8(maxSpeed);
        cur->vx = cur->vx / guess * UNFIX8(maxSpeed);
    }

    //adds the boid's x and y velocities to its x and y coordinates
    cur->x += cur->vx;
    cur->y += cur->vy;

    //wraps a boid around to the other side of the screen if it reaches an edge
    if (cur->x < 0) {
        cur->x = FIX8(240);
    } else if (cur->x > FIX8(240)) {
        cur->x = 0;
    }
    if (cur->y < 0) {
        cur->y = FIX8(160);
    } else if (cur->y > FIX8(160)) {
        cur->y = 0;
    }
}


