#include "myLib.h"
#include <stdlib.h>

unsigned short *videoBuffer = (unsigned short *)0x06000000;

//sets a single pixel in the video buffer to a given color value
void setPixel(int x, int y, unsigned short color) {
    videoBuffer[OFFSET(x, y, 240)] = color;
}

//fills the screen with a single color using DMA and a fixed color source
void drawBg3(const volatile u16 color) {
    DMA[DMA_CHANNEL_3].src = &color;
    DMA[DMA_CHANNEL_3].dst = videoBuffer;
    DMA[DMA_CHANNEL_3].cnt = (240 * 160)
                             | DMA_SOURCE_FIXED
                             | DMA_DESTINATION_INCREMENT
                             | DMA_ON;
}

//draws a rectangle with a given x, y, width, height, and color using DMA
void drawRect3(int x, int y, int width, int height, const u16 color) {
    for (int j = 0; j < height; j++) {
        DMA[DMA_CHANNEL_3].src = &color;
        DMA[DMA_CHANNEL_3].dst = videoBuffer + x + (y + j) * 240;
        DMA[DMA_CHANNEL_3].cnt = (width)
                                | DMA_SOURCE_FIXED
                                | DMA_DESTINATION_INCREMENT
                                | DMA_ON;
    }
}

//draws an image with a given x, y, width, and height using source incremented DMA
void drawImage3(int x, int y, int width, int height, const u16* image) {
    for (int j = 0; j < height; j++) {
        DMA[DMA_CHANNEL_3].src = image + j * width;
        DMA[DMA_CHANNEL_3].dst = videoBuffer + x + (y + j) * 240;
        DMA[DMA_CHANNEL_3].cnt = (width)
                                | DMA_SOURCE_INCREMENT
                                | DMA_DESTINATION_INCREMENT
                                | DMA_ON;
    }
}

//waits until drawing ends, then waits until drawing begins 
void waitForVblank() {
    while(SCANLINECOUNTER > 160);
    while(SCANLINECOUNTER < 160);
}
