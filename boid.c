//file defining the boid structure
#include "myLib.h"
#include <stdlib.h>

int maxSpeed = FIX8(4) / 3;
int numBoids = 16;
int cohesion = 12;
int sync = 3;
int repulsion = 4;
int viewRadius = FIX8(50);

//NOTE: FIX8 and UNFIX8 convert an int value to and from a fixed point decimal
//value. This shows up when converting decimal boid coordinates and velocities
//to integer values for rendering

//calculates the location that a boid needs to move to based on 5 rules
void applyBoidRules(BOID* cur, BOID others[], PLAYER player, FOOD food) {
    //get all of the neighbors of cur
    int* neighbors = getNeighbors(*cur, others);
    //apply the cohesion, repulsion, synchronization, food attraction, and
    //player repulsion rules to the current boid
    rule1(cur, others, neighbors);
    rule2(cur, others, neighbors);
    rule3(cur, others, neighbors);
    rule4(cur, food);
    rule5(cur, player);
    //release the memory for the neighbors array, since it is no longer needed
    free(neighbors);
}

//returns an integer array to represent which boids in "others" are neighbors of
//cur, e.g., if "others" is an array of 3 boids and cur is within viewing range
//of others[1] and others[2], getNeighbors returns [0, 1, 1]
int* getNeighbors(BOID cur, BOID others[]) {
    //dynamically allocate a pointer to an array of the size of potential
    //neighbors
    int* neighbors = calloc(sizeof(int), numBoids);
    //if the square of the distance from cur to another boid is less than the
    //square of a defined viewing radius and the other boid is not the current
    //boid, put a 1 at that location in neighbors to indicate that the two
    //boids are neighbors
    for (int i = 0; i < numBoids; i++) {
        if (distSqr(cur, others[i]) < SQR(viewRadius) && &others[i] != &cur) {
            neighbors[i] = 1;
        }
    }

    return neighbors;
}

//calculates the square of the distance between two boids
int distSqr(BOID a, BOID b) {
    return SQR(a.x - b.x) + SQR(a.y - b.y);
}

//cohesion rule; move the boid toward the center of mass of all of its neighbors
void rule1(BOID* cur, BOID others[], int neighbors[]) {
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
    ax /= cohesion;
    ay /= cohesion;

    //add the acceleration vector to the boid's velocity
    cur->vx += ax;
    cur->vy += ay;
}

//repulsion rule; attempt to move the boid to a safe distance from its neighbors
void rule2(BOID* cur, BOID others[], int neighbors[]) {
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
        tx /= repulsion;
        ty /= repulsion;

        //add the temp vector to the total acceleration vector
        ax -= tx;
        ay -= ty;
    }

    //add the acceleration vector to the current velocity
    cur->vx += ax;
    cur->vy += ay;
}

//sync rule; attempt to make a boid's velocity similar to those of its neighbors
void rule3(BOID* cur, BOID others[], int neighbors[]) {
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
    ax /= sync;
    ay /= sync;

    //add the acceleration vector to the current boid's velocity
    cur->vx += ax;
    cur->vy += ay;
}

//find food rule; make the boid move toward the food if the food is withing 4
//times the viewing radius
void rule4(BOID* cur, FOOD food) {
    if (SQR(cur->x - FIX8(food.x)) < SQR(viewRadius)
        && SQR(cur->y - FIX8(food.y)) < SQR(viewRadius * 2)) {
        cur->vx += (FIX8(food.x) - cur->x) / cohesion * 2;
        cur->vy += (FIX8(food.y) - cur->y) / cohesion * 2;
    }
}

//avoid player rule; make the boid move away from the player if the player is
//within the view radius
void rule5(BOID* cur, PLAYER player) {
    if (SQR(cur->x - FIX8(player.x)) < SQR(viewRadius)
        && SQR(cur->y - FIX8(player.y)) < SQR(viewRadius)) {
        cur->vx -= (FIX8(player.x) - cur->x) / (repulsion) / 10;
        cur->vy -= (FIX8(player.y) - cur->y) / (repulsion) / 10;
    }
}

//moves a boid's position by its x and y velocities
void moveBoid(BOID* cur) {
    //limits a boid's velocity to the maximum velocity value
    if (cur->vx > maxSpeed) {
        cur->vx = maxSpeed;
    } else if (cur->vx < -maxSpeed) {
        cur->vx = -maxSpeed;
    }
    if (cur->vy > maxSpeed) {
        cur->vy = maxSpeed;
    } else if (cur->vy < -maxSpeed) {
        cur->vy = -maxSpeed;
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

//calculates the new location of each living boid based on the results of each
//of the boid rules, then updates all of their positions simultaneously
void moveAllBoids(BOID boids[], PLAYER player, FOOD food) {
    //calculates new locations for all living boids
    for (int i = 0; i < numBoids; i++) {
        if (boids[i].isAlive) {
            applyBoidRules(&boids[i], boids, player, food);
        }
    }

    //updates each boid's previous position with its current position, then
    //updates its position to the position calculated in the previous loop
    for (int i = 0; i < numBoids; i++) {
        if (boids[i].isAlive) {
            boids[i].oldx = boids[i].x;
            boids[i].oldy = boids[i].y;
            moveBoid(&boids[i]);
        }
    }
}

//draws a boid centered at its x and y location with a "head" drawin in the
//direction of the boid's velocity
void drawBoid(BOID cur) {
    drawRect3((UNFIX8(cur.x) - 2), (UNFIX8(cur.y) - 2), 4, 4, cur.color);
    drawRect3((UNFIX8(cur.x) + UNFIX8(cur.vx * 2) - 1),
        (UNFIX8(cur.y) + UNFIX8(cur.vy * 2) - 1), 3, 3, cur.color);
}

//Draws a rectangle that covers the old location of a boid's body and head
void clearBoid(BOID cur) {
    drawRect3((UNFIX8(cur.oldx) - 4), (UNFIX8(cur.oldy) - 4), 8, 8, BG_COLOR);
}

//Clears all of the boids that are currently alive
void clearAllBoids(BOID boids[]) {
    for (int i = 0; i < numBoids; i++) {
        clearBoid(boids[i]);
    }
}

//Draws all of the boids to the screen that are currently alive
void drawAllBoids(BOID boids[]) {
    for (int i = 0; i < numBoids; i++) {
        if (boids[i].isAlive) {
            drawBoid(boids[i]);
        }
    }
}
