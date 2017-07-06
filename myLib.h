#ifndef MYLIB_H
    #define MYLIB_H
//TYPES
typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;


//Type definition for a boid, which attempts to flock with its neighbors and
//move toward food goals
//all fields are fixed point values
typedef struct {
    int x;
    int y;
    int oldx;
    int oldy;
    int vx;
    int vy;
    int isAlive;
    u16 color;
} BOID;

//Type definition for the player that the user controls
typedef struct {
    int x;
    int y;
    int oldx;
    int oldy;
    int dir;
} PLAYER;

//Type definition for the food that the boids and the players eat
typedef struct {
    int x;
    int y;
} FOOD;

//boilerplate for DMA use
typedef struct {
    const volatile void *src;
    volatile void *dst;
    volatile u32 cnt;
} DMA_CONTROLLER;

//Definitions of all game states
typedef enum {START, START_NODRAW, OPTIONS, OPTIONS_NODRAW, UPDATE_OPTIONS,
    LEVEL_1, VICTORY, VICTORY_NODRAW, GAME_OVER, GAME_OVER_NODRAW} state;
state handleState(state);


//fixed point operators
#define FIX4(a) ((a) << 4)
#define UNFIX4(a) ((a) >> 4)

#define FIX8(a) ((a) << 8)
#define UNFIX8(a) ((a) >> 8)

//SCORING DEFINES
extern int boidScore;
extern int playerScore;
extern int boidsEaten;

//BOID DEFINES
#define SQR(a) ((a) * (a))

extern int maxSpeed;
extern int numBoids;
extern int cohesion;
extern int sync;
extern int repulsion;
extern int viewRadius;

void moveBoid(BOID*);
void moveAllBoids(BOID* boids, PLAYER* player, FOOD* food);
void clearAllBoids(BOID* boids);
void drawAllBoids(BOID* boids);
void rule1(BOID* cur, BOID* others, int* neighbors);
void rule2(BOID* cur, BOID* others, int* neighbors);
void rule3(BOID* cur, BOID* others, int* neighbors);
void rule4(BOID* cur, FOOD* food);
void rule5(BOID* cur, PLAYER* player);
int distSqr(BOID*, BOID*);
void drawBoid(BOID*);
void drawBoid2d(BOID*);
void clearBoid(BOID*);


//PLAYER DEFINES
extern int playerSpeed;

void movePlayer(PLAYER* player);
void clearPlayer(PLAYER player);
void drawPlayer(PLAYER player);
void handleBoidCollision(PLAYER player, BOID boids[]);


//FOOD DEFINES
void moveFood(FOOD* food);
void drawFood(FOOD food);
void handleFoodCollision(PLAYER player, BOID boids[], FOOD* food);


//GENERAL GRAPHICS DEFINES

#define BG_COLOR WHITE

#define REG_DISPCTL *(unsigned short *)0x4000000
#define MODE3 3
#define BG2_ENABLE (1<<10)

#define SCANLINECOUNTER *(volatile unsigned short *) 0x4000006

#define COLOR(r, g, b) ((r) | (g)<<5 | (b)<<10)
#define BLACK COLOR(0, 0, 0)
#define RED COLOR(31,0,0)
#define GREEN COLOR(0,31,0)
#define BLUE COLOR(0,0,31)
#define YELLOW COLOR(31,31,0)
#define CYAN COLOR(0, 31, 31)
#define MAGENTA COLOR(31, 0, 31)
#define WHITE COLOR(31, 31, 31)
#define GRAY COLOR(16, 16, 16)

#define OFFSET(x, y, rowlen) ((x) + ((y) * (rowlen)))

extern unsigned short *videoBuffer;

void setPixel(int x, int y, unsigned short color);
void drawBg3(volatile const u16 color);
void drawRect3(int x, int y, int width, int height, const u16 color);
void drawImage3(int x, int y, int width, int height, const u16* image);
void waitForVblank();

//Button definitions
#define BUTTON_A		(1<<0)
#define BUTTON_B		(1<<1)
#define BUTTON_SELECT	(1<<2)
#define BUTTON_START	(1<<3)
#define BUTTON_RIGHT	(1<<4)
#define BUTTON_LEFT		(1<<5)
#define BUTTON_UP		(1<<6)
#define BUTTON_DOWN		(1<<7)
#define BUTTON_R		(1<<8)
#define BUTTON_L		(1<<9)

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)

#define BUTTONS *(volatile unsigned int *)0x4000130


//DMA DEFINES
#define DMA ((volatile DMA_CONTROLLER*) 0x40000B0)
#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)
#define DMA_DESTINATION_RESET (3 << 21)

#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_REPEAT (1 << 25)

#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)
#define DMA_ON (1 << 31)

#endif
