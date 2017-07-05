extern const unsigned char fontdata_6x8[12288];

void drawChar(int x, int y, char ch, u16 color);
void drawString(int x, int y, char *str, u16 color);
void dmaChar(int x, int y, char ch);
void dmaString(int x, int y, char *str);
