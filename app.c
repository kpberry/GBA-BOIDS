//BOIDS
//A SEMI-IMMERSIVE 2D EXPERIENCE BY KEVIN BERRY

#include <stdlib.h>
#include <stdio.h>
#include "myLib.h"
#include "text/text.h"
#include "images/birds.h"
#include "images/majestic_eagle.h"
#include "food.h"

//menu variables
static int cursorPosition = 0;
//these variables keep track of key presses to prevent multiple inputs from
//the same keypress; every time menuHeld is set to 1, it is preventing another
//menu from interpreting the current menu option as a command
//keyHeld signifies that a keypress that does not navigate from a menu should
//be counted as just one keypress
static int keyHeld = 0;
static int menuHeld = 0;

//scoring variables
int boidScore = 0;
int playerScore = 0;
int boidsEaten = 0;

int main() {
    REG_DISPCTL = MODE3 | BG2_ENABLE;

    state curState = START;

    while (1) {
        //keep setting the state based on the result of actions taking in the
        //previous state
        curState = handleState(curState);
    }
}

//handles all of the possible game states
state handleState(state curState) {
    if (curState  == START) {
        //draw the majestic background image
        waitForVblank();
        drawImage3(0, 0, MAJESTIC_EAGLE_WIDTH, MAJESTIC_EAGLE_HEIGHT, (u16*) &majestic_eagle);
        //draw the text
        waitForVblank();
        drawString(10, 10, "BOIDS", RED);
        drawString(10, 24, "THE GAME", COLOR(20, 0, 0));
        drawString(10, 130, "A: Begin Game", RED);
        drawString(10, 144, "B: Options", RED);
        return START_NODRAW;
    } else if (curState == START_NODRAW) {
        //if there was not a button pressed in the previous frame, responds to
        //START or A by starting the game, and to SELECT by opening the menu
        if (!menuHeld) {
            if (KEY_DOWN_NOW(BUTTON_A)) {
                menuHeld = 1;
                return LEVEL_1;
            } else if (KEY_DOWN_NOW(BUTTON_B)) {
                menuHeld = 1;
                return OPTIONS;
            } else {
                return START_NODRAW;
            }
        } else {
            //if there is no button pressed, sets menu held to false
            if (!KEY_DOWN_NOW((BUTTON_SELECT | BUTTON_START | BUTTON_A))) {
                menuHeld = 0;
            }
            //repeats the state until a valid button is pressed
            return START_NODRAW;
        }
    } else if (curState == OPTIONS) {
        //indicate that the menu was opened in the previous frame
        menuHeld = 1;
        cursorPosition = 0;

        //these strings store the variables that the user can change
        char valString1[24];
        char valString2[24];
        char valString3[24];
        char valString4[24];
        char valString5[24];
        char valString6[24];

        //converts the user modifiable variables to strings
        sprintf(valString1, "%d", numBoids);
        sprintf(valString2, "%d", cohesion);
        sprintf(valString3, "%d", sync);
        sprintf(valString4, "%d", repulsion);
        sprintf(valString5, "%d", playerSpeed);
        sprintf(valString6, "%d", UNFIX8(viewRadius));

        waitForVblank();
        drawBg3(BLACK);

        //Draw all of the labels
        drawString(240 / 2 - 8 * 3.5, 10, "OPTIONS", WHITE);

        drawString(10, 30,  "NUMBER OF BOIDS:", WHITE);
        drawString(10, 44,  " COHESION VALUE:", WHITE);
        drawString(10, 58,  "     SYNC VALUE:", WHITE);
        drawString(10, 72,  "REPULSION VALUE:", WHITE);
        drawString(10, 86,  "   PLAYER SPEED:", WHITE);
        drawString(10, 100, "    VIEW RADIUS:", WHITE);
        drawString(10, 130, "Start: Begin Game", BLUE);
        drawString(10, 144, "Select: Main Menu", BLUE);

        //draw all of the values
        drawString(180, 30, "<", WHITE);
        drawString(150, 30, valString1, WHITE);
        drawString(150, 44, valString2, WHITE);
        drawString(150, 58, valString3, WHITE);
        drawString(150, 72, valString4, WHITE);
        drawString(150, 86, valString5, WHITE);
        drawString(150, 100, valString6, WHITE);

        return OPTIONS_NODRAW;
    } else if (curState == OPTIONS_NODRAW) {
        //if the menu was not transitioned to in the previous frame, listens for
        //button input
        if (!menuHeld) {
            if (KEY_DOWN_NOW(BUTTON_SELECT)) { //return to main menu
                menuHeld = 1;
                return START;
            } else if (KEY_DOWN_NOW(BUTTON_DOWN)) { //move cursor down
                cursorPosition = (cursorPosition + 1) % 6;
                return UPDATE_OPTIONS;
            } else if (KEY_DOWN_NOW(BUTTON_UP)) { //move cursor up
                cursorPosition = (cursorPosition + 5) % 6;
                return UPDATE_OPTIONS;
            } else if (KEY_DOWN_NOW(BUTTON_LEFT)) { //decrement value at cursor
                if (cursorPosition == 0) {
                    numBoids = (numBoids - 1 + 100) % 100;
                } else if (cursorPosition == 1) {
                    cohesion = (cohesion - 1) % 30;
                } else if (cursorPosition == 2) {
                    sync = (sync - 1) % 30;
                } else if (cursorPosition == 3) {
                    repulsion = (repulsion - 1) % 30;
                } else if (cursorPosition == 4) {
                    playerSpeed = (playerSpeed - 1 + 5) % 5;
                } else if (cursorPosition == 5) {
                    viewRadius = (viewRadius - FIX8(1) + FIX8(100)) % FIX8(100);
                }
                return UPDATE_OPTIONS;
            } else if (KEY_DOWN_NOW(BUTTON_RIGHT)) { //increment value at cursor
                if (cursorPosition == 0) {
                    numBoids = (numBoids + 1 + 100) % 100;
                } else if (cursorPosition == 1) {
                    cohesion = (cohesion + 1) % 30;
                } else if (cursorPosition == 2) {
                    sync = (sync + 1) % 30;
                } else if (cursorPosition == 3) {
                    repulsion = (repulsion + 1) % 30;
                } else if (cursorPosition == 4) {
                    playerSpeed = (playerSpeed + 1 + 5) % 5;
                } else if (cursorPosition == 5) {
                    viewRadius = (viewRadius + FIX8(1 + 100)) % FIX8(100);
                }
                return UPDATE_OPTIONS;
            } else if (KEY_DOWN_NOW(BUTTON_START)) { //start the game
                menuHeld = 1;
                return LEVEL_1;
            } else { //otherwise, keep listening for menu input
                return OPTIONS_NODRAW;
            }
        } else { //if no key is pressed, start listening for input
            if (!(KEY_DOWN_NOW(BUTTON_DOWN) | KEY_DOWN_NOW(BUTTON_UP)
                | KEY_DOWN_NOW(BUTTON_LEFT) | KEY_DOWN_NOW(BUTTON_RIGHT)
                | KEY_DOWN_NOW(BUTTON_START) | KEY_DOWN_NOW(BUTTON_SELECT))) {
                menuHeld = 0;
            }
            return OPTIONS_NODRAW;
        }
    } else if (curState == UPDATE_OPTIONS) {
        //state that only updates an option and its neighbors instead of all of
        //the options at once when the user changes an option or moves the
        //cursor to prevent tearing
        menuHeld = 1;

        //string to store the variable that needs to be visually updated
        char valString[28];
        if (cursorPosition == 0) {
            //write a string with the new number of boids
            sprintf(valString, "%d", numBoids);
            waitForVblank();
            //clears the cursor from the possible neighboring squares
            drawRect3(150, 30, 40, 8, BLACK);
            drawRect3(180, 44, 40, 8, BLACK);
            drawRect3(180, 58, 40, 8, BLACK);
            drawRect3(180, 100, 40, 8, BLACK);
            //draws the cursor and the new boid number
            drawString(150, 30, valString, WHITE);
            drawString(180, 30, "<", WHITE);
        }

        if (cursorPosition == 1) {
            //write a string with the new cohesion value
            sprintf(valString, "%d", cohesion);
            waitForVblank();
            //clears the cursor from the possible neighboring squares
            drawRect3(180, 30, 40, 8, BLACK);
            drawRect3(150, 44, 40, 8, BLACK);
            drawRect3(180, 58, 40, 8, BLACK);
            //draws the cursor and the new cohesion value
            drawString(150, 44, valString, WHITE);
            drawString(180, 44, "<", WHITE);
        }

        if (cursorPosition == 2) {
            //write a string with the new sync value
            sprintf(valString, "%d", sync);
            waitForVblank();
            //clears the cursor from the possible neighboring squares
            drawRect3(180, 44, 40, 8, BLACK);
            drawRect3(150, 58, 40, 8, BLACK);
            drawRect3(180, 72, 40, 8, BLACK);
            //draws the cursor and the new sync value
            drawString(150, 58, valString, WHITE);
            drawString(180, 58, "<", WHITE);
        }

        if (cursorPosition == 3) {
            //write a string with the new repulsion value
            sprintf(valString, "%d", repulsion);
            waitForVblank();
            //clears the cursor from the possible neighboring squares
            drawRect3(180, 58, 40, 8, BLACK);
            drawRect3(150, 72, 40, 8, BLACK);
            drawRect3(180, 86, 40, 8, BLACK);
            //draws the cursor and the new repulsion value
            drawString(150, 72, valString, WHITE);
            drawString(180, 72, "<", WHITE);
        }

        if (cursorPosition == 4) {
            //write a string with the new player speed
            sprintf(valString, "%d", playerSpeed);
            waitForVblank();
            //clears the cursor from the possible neighboring squares
            drawRect3(180, 72, 40, 8, BLACK);
            drawRect3(150, 86, 40, 8, BLACK);
            drawRect3(180, 100, 40, 8, BLACK);
            //draws the cursor and the new player speed
            drawString(150, 86, valString, WHITE);
            drawString(180, 86, "<", WHITE);
        }

        if (cursorPosition == 5) {
            //write a string with the new view radius
            sprintf(valString, "%d", UNFIX8(viewRadius));
            waitForVblank();
            //clears the cursor from the possible neighboring squares
            drawRect3(180, 30, 40, 8, BLACK);
            drawRect3(180, 86, 40, 8, BLACK);
            drawRect3(150, 100, 40, 8, BLACK);
            //draws the cursor and the new view radius
            drawString(150, 100, valString, WHITE);
            drawString(180, 100, "<", WHITE);
        }

        //return to the option listening state
        return OPTIONS_NODRAW;
    } else if (curState == LEVEL_1) {
        //array to hold all of the boids
        BOID boids[numBoids];
        //initialize player and food
        PLAYER player = {.x = 120, .y = 80, .oldx = 120, .oldy = 80};
        FOOD food = {.x = 200, .y = 130};

        //strings for displaying player and boid scores
        char boidScoreString[20];
        char playerScoreString[20];

        //reset scores
        boidScore = 0;
        playerScore = 0;

        //initialize boids with random positions, velocities, colors
        for (int i = 0; i < numBoids; i++) {
            boids[i] =  (BOID) {.x = rand() % FIX8(200),
                                .y = 16 + rand() % FIX8(144),
                                .vx = rand() % FIX8(2),
                                .vy = rand() % FIX8(2),
                                .isAlive = 1,
                                .color = COLOR(rand() % 31,
                                               rand() % 11,
                                               rand() % 10)};
        }

        waitForVblank();
        drawBg3(BG_COLOR);

        while(1) {
            //game over and victory conditions
            if (boidScore >= 100) {
                return GAME_OVER;
            } else if (playerScore >= 20) {
                return VICTORY;
            }

            //listeners for openting start menu or opening options menu
            if (!menuHeld && KEY_DOWN_NOW(BUTTON_START)) {
                menuHeld = 1;
                return OPTIONS;
            } else if (!menuHeld && KEY_DOWN_NOW(BUTTON_SELECT)) {
                menuHeld = 1;
                return START;
            } else if (!KEY_DOWN_NOW((BUTTON_START | BUTTON_SELECT))) {
                //indicates that no menu keys are being pressed
                menuHeld = 0;
            }

            //listens for arrow key presses; interprets the results as a one
            //hot state machine, i.e., each value 0-7 corresponds to a different
            //diagonal direction
            if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
                keyHeld = 1;
                if (KEY_DOWN_NOW(BUTTON_UP)) {
                    player.dir = 4;
                } else if (KEY_DOWN_NOW(BUTTON_DOWN)) {
                    player.dir = 5;
                } else {
                    player.dir = 0;
                }
            } else if (KEY_DOWN_NOW(BUTTON_DOWN)) {
                keyHeld = 1;
                if (KEY_DOWN_NOW(BUTTON_LEFT)) {
                    player.dir = 7;
                } else if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
                    player.dir = 5;
                } else {
                    player.dir = 1;
                }
            } else if (KEY_DOWN_NOW(BUTTON_LEFT)) {
                keyHeld = 1;
                if (KEY_DOWN_NOW(BUTTON_UP)) {
                    player.dir = 6;
                } else if (KEY_DOWN_NOW(BUTTON_DOWN)) {
                    player.dir = 7;
                } else {
                    player.dir = 2;
                }
            } else if (KEY_DOWN_NOW(BUTTON_UP)) {
                keyHeld = 1;
                if (KEY_DOWN_NOW(BUTTON_LEFT)) {
                    player.dir = 6;
                } else if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
                    player.dir = 4;
                } else {
                    player.dir = 3;
                }
            } else {
                //indicates that no directional keys were pressed
                keyHeld = 0;
            }

            //if the user is currently holding a key, moves the player
            if (keyHeld) {
                movePlayer(&player);
            }

            //moves each bird, checks if they any of them ate the food, then
            //checks if any of them were passed over by the player,
            moveAllBoids(boids, player, food);
            handleFoodCollision(player, boids, &food);
            handleBoidCollision(player, boids);
            //prints the number of food items eaten by the player (P) and the
            //boids (B)
            sprintf(boidScoreString, "Boids:%d", boidScore);
            sprintf(playerScoreString, "Player:%d", playerScore);

            waitForVblank();
            //clears the player and boids from the screen
            clearPlayer(player);
            clearAllBoids(boids);
            //draws the food, boids, and the player
            //the food has to be drawn first since it contains white pixels at
            //its edges
            drawFood(food);
            drawAllBoids(boids);
            drawPlayer(player);

            //draw the scores last so that they go over the other objects
            //scores drawn with dma to prevent tearing
            dmaString(1, 1, playerScoreString);
            dmaString(1, 14, boidScoreString);
        }

        //this code will never be reached, but has to be here to satisfy the
        //compiler
        return START;
    } else if (curState == GAME_OVER) {
        //draws the game over screen
        waitForVblank();
        drawImage3(0, 0, BIRDS_WIDTH, BIRDS_HEIGHT, (u16*) &birds);
        //draws the text and instructions
        drawString(240 / 2 - 8 * 4.5, 10, "GAME OVER", RED);
        drawString(240 / 2 - 8 * 9.5, 30, "Alfred Hitchcock is", RED);
        drawString(240 / 2 - 8 * 6, 44, "not impressed.", RED);
        drawString(10, 130, "Select: Main Menu", RED);
        drawString(10, 144, "Start: Begin Game", RED);
        return GAME_OVER_NODRAW;
    } else if (curState == GAME_OVER_NODRAW || curState == VICTORY_NODRAW) {
        //listens for input to return to the menu or start a new game
        if (KEY_DOWN_NOW((BUTTON_SELECT | BUTTON_B))) {
            menuHeld = 1;
            return START;
        } else if (KEY_DOWN_NOW((BUTTON_A | BUTTON_START))) {
            menuHeld = 1;
            return LEVEL_1;
        } else {
            return GAME_OVER_NODRAW;
        }
    } else if (curState == VICTORY) {
        //draws the victory screen
        waitForVblank();
        drawImage3(0, 0, BIRDS_WIDTH, BIRDS_HEIGHT, (u16*) &birds);
        //draws the congratulatory text
        drawString(240 / 2 - 8 * 4.5, 10, " VICTORY!", BLUE);
        drawString(240 / 2 - 8 * 9.5, 30, "Alfred Hitchcock is", BLUE);
        drawString(240 / 2 - 8 * 7, 44, "QUITE impressed!", BLUE);
        drawString(10, 130, "Select: Main Menu", BLUE);
        drawString(10, 144, "Start: Begin Game", BLUE);
        return GAME_OVER_NODRAW;
    } else {
        //this code will also never be reached, but has to exist to satisfy the
        //compiler
        return START;
    }
}
