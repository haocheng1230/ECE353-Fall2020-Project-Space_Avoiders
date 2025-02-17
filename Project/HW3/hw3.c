#include "hw3.h"
#include "main.h"
#define LCD_X_MAX 239 // 0-239
#define LCD_Y_MAX 319 // 0-319
//extern bool game_start = false;
bool game_start = false;
volatile uint16_t FIGHTER_X_COORD = 190;
volatile uint16_t FIGHTER_Y_COORD = 270;
volatile uint32_t CURRENT_SCORE = 0;
volatile uint16_t INVADER_X_COORD = 50;
volatile uint16_t INVADER_Y_COORD = 40;

char STUDENT_NAME[] = "Shuai Zhang & Haocheng Chen";
extern volatile bullet bullets[BULLET_NUM];
volatile bool ALERT_BULLET = true;
volatile bool ALERT_FIGHTER = true;
volatile bool ALERT_INVADER = true;

volatile uint8_t HIT_POINT = 0xFF;
volatile bool INVINCIBLE = false;
volatile bool ULT = false;
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
  io_expander_init();
	
  // Update the Space Shipt 60 times per second.
	gp_timer_config_32(TIMER1_BASE,TIMER_TAMR_TAMR_PERIOD, 50000000, false, true);
  gp_timer_config_32(TIMER2_BASE,TIMER_TAMR_TAMR_PERIOD, 610000, false, true);
  gp_timer_config_32(TIMER3_BASE,TIMER_TAMR_TAMR_PERIOD, 770000, false, true);
  gp_timer_config_32(TIMER4_BASE,TIMER_TAMR_TAMR_PERIOD, 50000, false, true);
	gp_timer_config_32(TIMER5_BASE,TIMER_TAMR_TAMR_PERIOD, 1400000, false, true);
}

//*****************************************************************************
// Main application for HW3
//*****************************************************************************
void hw3_main(void)
{
    bool game_over = false;
    int i = 0;
		uint32_t highscore;
		char temp[100];
    init_hardware();
		
		highscore = eeprom_score_read();
		sprintf(temp, "%X", highscore);
		draw_string("HIGH SCORE", 30,80, LCD_COLOR_BLUE);
		draw_string(temp, 40, 120, LCD_COLOR_RED);
	
		// pull the LCD for game start signal
    while(!game_start){
			uint16_t x;
			uint16_t y;
			char temp[100];
			lcd_draw_image(
				LCD_X_MAX/2,
				touchToStartWidthPixels,
				LCD_Y_MAX/2,
				touchToStartHeightPixels,
				touchToStartBitmaps,
				LCD_COLOR_RED,
				LCD_COLOR_BLACK
			);
			x = ft6x06_read_x();
			y = ft6x06_read_y();

			
			if ( x >= 1 && x <= LCD_X_MAX && y <= LCD_Y_MAX && y >= 1) {
				// LCD touched
				game_start = true;

				// display hitpoint LED
				io_expander_write_reg(MCP23017_GPIOA_R, HIT_POINT);
				lcd_clear_screen(LCD_COLOR_BLACK);
				CURRENT_SCORE = 0;
			}
		
		} // wait until touch the screen to start game
		
		// player can perform ULT at start
		ULT = true;
		
    while(!game_over) {
			  int width = 0;
				
				if (ALERT_BULLET) { // if we can move bullet
					ALERT_BULLET = false;
					
					// refersh bullet position
		for (i = 0 ; i < BULLET_NUM ; i++) {
            if (bullets[i].draw == true) {
                if (bullets[i].map == bulletBitmaps) {
                    width = 5; 
                } else {
                    width = 12;
                }
                lcd_draw_image(
                    bullets[i].x,
                    width,
                    bullets[i].y,
                    12,
                    bullets[i].map,
                    bullets[i].color,
									  LCD_COLOR_BLACK
                );
            }
				
				//check if the bullet has hit the fighter
				if (check_game_over(FIGHTER_X_COORD,FIGHTER_Y_COORD,fighterHeightPixel,fighterWidthPixel,
            bullets[i].x,bullets[i].y,width,12)) { // if we hit a bullet
							if (!INVINCIBLE) {
								INVINCIBLE = 1;
								HIT_POINT  = HIT_POINT >> 1;
								ULT = true;
							}
						}
        }
				}
				// game over if play lost all HP
			if (HIT_POINT == 0){
				game_over = true;
			}
		//step to infinate loop
	if(game_over) {
							break;
		}		
				
		// check if the player have collide with the invader		
	 game_over = check_game_over(
                                        FIGHTER_X_COORD,
                                        FIGHTER_Y_COORD,
                                        fighterHeightPixel,
                                        fighterWidthPixel,
                                        INVADER_X_COORD,
                                        INVADER_Y_COORD,
                                        invaderHeightPixels,
                                        invaderWidthPixels
                                    );		
				//step to infinate loop
				if(game_over) {
							break;
		}		
				// update fighter's position and draw on LCD  screen
			if(ALERT_FIGHTER)
           {
             ALERT_FIGHTER = false;
            
             lcd_draw_image(
                           FIGHTER_X_COORD,          // X Center Point
                           fighterWidthPixel,       // Image Horizontal Width
                           FIGHTER_Y_COORD,          // Y Center Point
                           fighterHeightPixel,      // Image Vertical Height
                           fighterBitmaps,           // Image
                           LCD_COLOR_RED,            // Foreground Color
                           LCD_COLOR_BLACK           // Background Color
                        );
              
           }
				
					// update invader's position and draw on LCD  screen
					 
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
          }
					 
					 
    } 
		
		// game over
		
		lcd_clear_screen(LCD_COLOR_BLACK);
		if (CURRENT_SCORE > highscore) eeprom_score_write(CURRENT_SCORE);
		sprintf(temp, "%X", CURRENT_SCORE);
		draw_string("GAME SCORE", 30,80, LCD_COLOR_BLUE);
		draw_string(temp, 40, 120, LCD_COLOR_YELLOW);
		draw_string("GAME OVER", 40, 160, LCD_COLOR_RED);


}
