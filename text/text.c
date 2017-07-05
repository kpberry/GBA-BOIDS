#include "../myLib.h"
#include "text.h"
#include "fontimage.h"

//draws a character given the x and y coordinates of its top left corner
void drawChar(int x, int y, char ch, u16 color) {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            if (fontdata_6x8[OFFSET(i, j, 6) + ch * 48]) {
                setPixel(x + i, y + j, color);
            }
        }
    }
}

//draws a string given the x and y coordinates of its top left corner
void drawString(int x, int y, char *str, u16 color) {
	while(*str) {
		drawChar(x, y, *str++, color);
		x += 8;
	}
}

//draws a black on white character with dma given the x and y coordinates of its
//top left corner
void dmaChar(int x, int y, char ch) {
    drawImage3(x, y, 6, 8, (u16*) &fontimage_6x8[ch * 48]);
}

//draws a black on white string with dma given the x and y coordinates of its
//top left corner
void dmaString(int x, int y, char *str) {
    while(*str) {
		dmaChar(x, y, *str++);
		x += 8;
	}
}
