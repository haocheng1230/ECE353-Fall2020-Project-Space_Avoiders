#include "hw3.h"
#define LCD_X_MAX 239 // 0-239
#define LCD_Y_MAX 319 // 0-319
volatile uint16_t SHIP_X_COORD = 190;
volatile uint16_t SHIP_Y_COORD = 270;
volatile uint16_t INVADER_X_COORD = 50;
volatile uint16_t INVADER_Y_COORD = 40;
volatile bool ALERT_SPACE_SHIP = true;
volatile bool ALERT_INVADER = true;
char STUDENT_NAME[] = "Shuai Zhang & Haocheng Chen";


typedef struct
{
    int top;
    int bottom;
    int left;
    int right;
} Rectangle;


//*****************************************************************************
// Determines if any part of the image would be off the screen if the image
// is moved in the specified direction.
//*****************************************************************************
bool contact_edge (
    volatile PS2_DIR_t direction,
    volatile uint16_t x_coord, 
    volatile uint16_t y_coord, 
    uint8_t image_height, 
    uint8_t image_width) {


    // determine edge contact for each case
    switch (direction) {
        case PS2_DIR_UP:
        if (y_coord - image_height/2 - 1 < 0) {
            return true; // contacted return true
        }
        break;

        case PS2_DIR_DOWN:
        if (y_coord + image_height/2 + 1 > LCD_Y_MAX) {
            return true;
        }
        break;

        case PS2_DIR_LEFT:
        if (x_coord - image_width/2 - 1 < 0) {
            return true;
        }
        break;

        case PS2_DIR_RIGHT:
        if (x_coord + image_width/2 + 1 > LCD_X_MAX) {
            return true;
        }
        break;

        default:
            break;
    }

    // if none above is triggered, then no edge contact
    return false;
}


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
        uint8_t image_width) {

    // move the coord of image by 1 base on direction
    switch (direction) {
        case PS2_DIR_UP:
            (*y_coord) -= 1;
            break;
        case PS2_DIR_DOWN:
            (*y_coord) += 1;
            break;
        case PS2_DIR_LEFT:
            (*x_coord) -= 1;
            break;
        case PS2_DIR_RIGHT:
            (*x_coord) += 1;
            break;
        default:
            break;
    }
   
}

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
        uint8_t invader_width) {


    uint16_t invader_U = invader_y_coord - invader_height/2; // invader top edge
    uint16_t invader_D = invader_y_coord + invader_height/2; // bottom edge
    uint16_t invader_L = invader_x_coord - invader_width/2; // left
    uint16_t invader_R = invader_x_coord + invader_width/2; // right

    uint16_t ship_U = ship_y_coord - ship_height/2; // same to above but for ship
    uint16_t ship_D = ship_y_coord + ship_height/2;
    uint16_t ship_L = ship_x_coord - ship_width/2;
    uint16_t ship_R = ship_x_coord + ship_width/2;


    if (invader_U < ship_D && ship_U < invader_D) { // confirm vertical overlap
        if (invader_L < ship_R && ship_L < invader_R) { // horizontal
            return true; // game over return true
        }
    }

    return false;

}

//*****************************************************************************
// Initializes all of the peripherls used in HW3
//*****************************************************************************
void init_hardware(void)
{
  lcd_config_gpio();
  lcd_config_screen();
  lcd_clear_screen(LCD_COLOR_BLACK);
  ps2_initialize();
  
  // Update the Space Shipt 60 times per second.
  gp_timer_config_32(TIMER2_BASE,TIMER_TAMR_TAMR_PERIOD, 1000000, false, true);
  gp_timer_config_32(TIMER3_BASE,TIMER_TAMR_TAMR_PERIOD, 500000, false, true);
  gp_timer_config_32(TIMER4_BASE,TIMER_TAMR_TAMR_PERIOD, 50000, false, true);
}

//*****************************************************************************
// Main application for HW3
//*****************************************************************************
void hw3_main(void)
{
    bool game_over = false;
    init_hardware();
   

      while(!game_over)
      {
          if(ALERT_SPACE_SHIP)
          {
            ALERT_SPACE_SHIP = false;
            
            lcd_draw_image(
                          SHIP_X_COORD,                       // X Center Point
                          space_shipWidthPixels,   // Image Horizontal Width
                          SHIP_Y_COORD,                       // Y Center Point
                          space_shipHeightPixels,  // Image Vertical Height
                          space_shipBitmaps,       // Image
                          LCD_COLOR_BLUE,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );

            game_over = check_game_over(
                                        SHIP_X_COORD,
                                        SHIP_Y_COORD,
                                        space_shipHeightPixels,
                                        space_shipWidthPixels,
                                        INVADER_X_COORD,
                                        INVADER_Y_COORD,
                                        invaderHeightPixels,
                                        invaderWidthPixels
                                    );
          }
          
          if(ALERT_INVADER)
          {
            ALERT_INVADER = false;
            
            lcd_draw_image(
                          INVADER_X_COORD,          // X Center Point
                          invaderWidthPixels,       // Image Horizontal Width
                          INVADER_Y_COORD,          // Y Center Point
                          invaderHeightPixels,      // Image Vertical Height
                          invaderBitmaps,           // Image
                          LCD_COLOR_RED,            // Foreground Color
                          LCD_COLOR_BLACK           // Background Color
                        );
              
             game_over = check_game_over(
                                            SHIP_X_COORD,
                                            SHIP_Y_COORD,
                                            space_shipHeightPixels,
                                            space_shipWidthPixels,
                                            INVADER_X_COORD,
                                            INVADER_Y_COORD,
                                            invaderHeightPixels,
                                            invaderWidthPixels
                                        );
          }
          
      }   
}
