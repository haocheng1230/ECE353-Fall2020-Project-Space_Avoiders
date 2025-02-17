#ifndef __HW3_H__
#define __HW3_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "TM4C123.h"
#include "gpio_port.h"
#include "ps2.h"
#include "lcd.h"
#include "timers.h"
#include "hw3_images.h"
#include "io_expander.h"
#include "display.h"
#include "scoreboard.h"

extern volatile uint16_t INVADER_X_COORD;
extern volatile uint16_t INVADER_Y_COORD;
extern volatile uint16_t FIGHTER_X_COORD;
extern volatile uint16_t FIGHTER_Y_COORD;
extern volatile bool ALERT_INVADER;
extern volatile bool ALERT_FIGHTER;
extern volatile bool ALERT_BULLET;
extern char STUDENT_NAME[];
extern volatile uint32_t CURRENT_SCORE; 

extern volatile uint8_t HIT_POINT;
extern volatile bool INVINCIBLE;
extern volatile uint16_t INVINCIBLE_TIMER_COUNT_DOWN;
extern volatile bool ULT;

extern volatile uint32_t MARK;



#define INVINCIBLE_TIME_MAX 1000;
#define BULLET_NUM 8

typedef enum{
  PS2_DIR_UP,
  PS2_DIR_DOWN,
  PS2_DIR_LEFT,
  PS2_DIR_RIGHT,
  PS2_DIR_CENTER,
  PS2_DIR_INIT,
} PS2_DIR_t;

typedef struct bullet
{
    const uint8_t* map;
    uint16_t x;
    uint16_t y;
    uint16_t xSpeed;
    uint16_t ySpeed;
    uint16_t color;
    bool draw;

} bullet;

//*****************************************************************************
// Determines if any part of the image would be off the screen if the image
// is moved in the specified direction.
//*****************************************************************************
bool contact_edge(
    volatile PS2_DIR_t direction,
    volatile uint16_t x_coord, 
    volatile uint16_t y_coord, 
    uint8_t image_height, 
    uint8_t image_width
);

//*****************************************************************************
// Moves the image by one pixel in the provided direction.  The second and 
// third parameter should modify the current location of the image (pass by
// reference)
//*****************************************************************************
void move_image(
        volatile PS2_DIR_t direction,
        volatile uint16_t *x_coord, 
        volatile uint16_t *y_coord, 
        uint8_t image_height, 
        uint8_t image_width
);

//*****************************************************************************
// Determines is any portion of the two images are overlapping.  An image is
// considered to be overlapping if the two rectangles determined by the image
// height and widths are overlapping.  An overlap occurs even if the area that
// overlaps are portions of the images where the pixels do not display on the
// screen.
//*****************************************************************************
bool check_game_over(
        volatile uint16_t ship_x_coord, 
        volatile uint16_t ship_y_coord, 
        uint8_t ship_height, 
        uint8_t ship_width,
        volatile uint16_t invader_x_coord, 
        volatile uint16_t invader_y_coord, 
        uint8_t invader_height, 
        uint8_t invader_width
);

//*****************************************************************************
// Initializes all of the peripherls used in HW3
//*****************************************************************************
void init_hardware(void);
        
        //*****************************************************************************
// Main application for HW3
//*****************************************************************************
void hw3_main(void);

#endif
