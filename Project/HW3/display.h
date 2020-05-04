#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "hw3_images.h"
#include <stdint.h>
#include <stdlib.h>
#include "lcd.h"


extern const uint8_t hyliaSerifBeta_18ptBitmaps[];
void draw_string(char* target, int x, int y, uint16_t color);

#endif
