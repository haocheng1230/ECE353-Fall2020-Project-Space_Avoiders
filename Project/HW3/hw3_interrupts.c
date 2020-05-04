#include "hw3_interrupts.h"
#include "hw3.h"
static volatile uint16_t PS2_X_DATA = 0;
static volatile uint16_t PS2_Y_DATA = 0;
static volatile PS2_DIR_t PS2_DIR = PS2_DIR_CENTER;

// new added globals
static volatile uint16_t MOVE_COUNT = 0;
static volatile PS2_DIR_t SPACE_SHIP_DIRECTION = PS2_DIR_CENTER;

volatile bullet bullets[BULLET_NUM];

static int BULLET_WIDTH = 12;
static int BULLET_HEIGHT = 12;
static int LCD_WIDTH = 240;
static int LCD_HEIGHT = 320;

volatile uint16_t INVINCIBLE_TIMER_COUNT_DOWN = INVINCIBLE_TIME_MAX;

volatile bool blink = true;
uint8_t button;

volatile uint32_t MARK = 0;

#define bounce_time_delay_defalut 5
volatile uint8_t bounce_delay = 0;
volatile bool bounce = false;

void spawn_bullet (volatile bullet* ptr){
	//int upper = 20;
	//int lower = 220;
	
	int bitMapResult = rand()%10;
	int directionResultX = rand()%3;
	int directionResultY = rand()%3;
	
	ptr->map = 		(bitMapResult == 0) ? bulletBitmaps :  			// blue meteorite
								(bitMapResult < 9) ? roundbulletBitmaps :		// green normal shell
								spreadbulletBitmaps;												// red anti-matter bomb, very dangerous and rare
	
	ptr->color = 	(bitMapResult == 0) ? LCD_COLOR_BLUE :
								(bitMapResult < 9) ? LCD_COLOR_GREEN :
								LCD_COLOR_RED;
	
	ptr->x =			(ptr->map == bulletBitmaps) ? (rand()%200 + 20) : 
								INVADER_X_COORD;
	
	ptr->y = 			(ptr->map == bulletBitmaps) ? BULLET_HEIGHT : 
								INVADER_Y_COORD;
	
	ptr->xSpeed =		(ptr->map == bulletBitmaps) ? 0 :
									(directionResultX == 0) ? -1 :
									(directionResultX == 1) ? 0 :
									1;
	
	ptr->ySpeed =	(ptr->map == bulletBitmaps) ? 1 :
								(directionResultY == 0) ? -1 :
								(directionResultY == 1) ? 0 :
								1;
								
	if (ptr->xSpeed == 0 && ptr->ySpeed == 0){
		ptr->draw = false;
	} else {
		ptr->draw = true;
	}
	//srand(0);
}

//*****************************************************************************
// Returns the most current direction that was pressed.
//*****************************************************************************
PS2_DIR_t ps2_get_direction(void)
{
    // determine if the joystick have reached threshold value
    // threshholds are 2.4V and 0.85V
      if (PS2_X_DATA > (0xfff/3.3*2.4)) {//The reading of the X direction is > 2.40V
        return PS2_DIR_LEFT;
    } else if (PS2_X_DATA < (0xfff/3.3*0.85)) {//The reading of the X direction is < 0.85V
        return PS2_DIR_RIGHT;
    } else if (PS2_Y_DATA > (0xfff/3.3*2.4)) {//The reading of the Y direction is > 2.40V
        return PS2_DIR_UP;
    } else if (PS2_Y_DATA < (0xfff/3.3*0.85)) {//The reading of the Y direction is < 0.85V
        return PS2_DIR_DOWN;
    } else if ((PS2_X_DATA > (0xfff/3.3*0.85)) & (PS2_X_DATA < (0xfff/3.3*0.24))) {
          //The reading of the X direction is > 0.85V
          //AND the X direction is < 0.240V
          return PS2_DIR_CENTER;
      } else {
        return PS2_DIR_CENTER;
    }
}



void GPIOF_Handler(){
	uint8_t i;
	
	io_expander_read_reg(MCP23017_GPIOB_R, &button);
	if (ULT) {	
		ULT = false;
		for (i = 0; i < BULLET_NUM; i++){
		bullets[i].draw = false;
	}
	lcd_clear_screen(LCD_COLOR_BLACK);
	}
	GPIOF->ICR |= GPIO_ICR_GPIO_M;
}


// flash hitpoint, as well as reset debounce;
// very slow
void TIMER1A_Handler(void)
{
	blink = !blink;
	if (!blink) {
		io_expander_write_reg(MCP23017_GPIOA_R, 0x00);
	} else {
		io_expander_write_reg(MCP23017_GPIOA_R, HIT_POINT);
	}
	TIMER1->ICR |= TIMER_ICR_TATOCINT;
	CURRENT_SCORE += 1;
}



//*****************************************************************************
// TIMER2 ISR is used to determine when to move the fighter
// Trigger every 20ms, priority 0
// The ISR will determine if the Invader
// image can be moved by 1 pixel in the current direction
// by calling the function contact_edge().

// If fighter can be moved, it will call the function move_image()
// to updated the global
// variables fighter_X_COORD and fighter_Y_COORD
// that hold the current location of the fighter.
//*****************************************************************************


void TIMER2A_Handler(void)
{
	
    // move invader if not contacting edge
    if (!contact_edge(PS2_DIR,
            FIGHTER_X_COORD,
            FIGHTER_Y_COORD,
            fighterHeightPixel,
            fighterWidthPixel
            )) {
        ALERT_FIGHTER = true;
    }

    // if the invader can be moved, then update its coord
    if (ALERT_FIGHTER) {
            move_image(
                    PS2_DIR,
                    &FIGHTER_X_COORD,
                    &FIGHTER_Y_COORD,
                    fighterHeightPixel,
                    fighterWidthPixel
                    );
    }
    // Clear the interrupt
	TIMER2->ICR |= TIMER_ICR_TATOCINT;
}



//*****************************************************************************
// TIMER3 ISR is used to update bullet
// Trigger every 10ms, priority 1
//*****************************************************************************

void TIMER3A_Handler(void)
{	
	volatile int i;

	if (!ALERT_BULLET) {
		// clear the bullet in the button of the screen
		for (i = 0; i < BULLET_NUM; i++){
			if (bullets[i].color == LCD_COLOR_BLACK) {
				bullets[i].draw = false;
			}
		}
		
		// show the bullet in the bottom as black
		for (i = 0; i < BULLET_NUM; i++){
			if (bullets[i].x < BULLET_WIDTH || bullets[i].x > (LCD_WIDTH - BULLET_WIDTH) || bullets[i].y > (LCD_HEIGHT - BULLET_HEIGHT) || bullets[i].y < BULLET_HEIGHT) {
				bullets[i].color = LCD_COLOR_BLACK;
			} 
		}
		
		for (i = 0; i < BULLET_NUM; i++){
		if (bullets[i].draw == false){
		spawn_bullet(&bullets[i]);
		} else {
			bullets[i].x += bullets[i].xSpeed;
			bullets[i].y += bullets[i].ySpeed;
		}
	}
		ALERT_BULLET = true;
	}

	
	// Clear the interrupt
	TIMER3->ICR |= TIMER_ICR_TATOCINT;  
}

//*****************************************************************************
// TIMER4 ISR is used to trigger the ADC
// Trigger every 1ms, priority 2
// also calculates invincible time 
//*****************************************************************************
void TIMER4A_Handler(void)
{
    // SS2 Initiate
    ADC0->PSSI |= ADC_PSSI_SS2;
	// Clear the interrupt
	TIMER4->ICR |= TIMER_ICR_TATOCINT; 
}

//*****************************************************************************
// ADC0 SS2 ISR
// update direction
//*****************************************************************************
void ADC0SS2_Handler(void)
{
    PS2_X_DATA = ADC0->SSFIFO2;
    PS2_Y_DATA = ADC0->SSFIFO2; // read x then y from FIFO
    PS2_DIR = ps2_get_direction();
	
		if (INVINCIBLE) {
			INVINCIBLE_TIMER_COUNT_DOWN -= 1;
		} 
		if (INVINCIBLE_TIMER_COUNT_DOWN == 0){
			INVINCIBLE = false;
			INVINCIBLE_TIMER_COUNT_DOWN = INVINCIBLE_TIME_MAX;
		}
	
    // Clear the interrupt
    ADC0->ISC |= ADC_ISC_IN2;
}

// update invader position, also update mark;
void TIMER5A_Handler(void)
{
    
     // update if out of moves
     if (MOVE_COUNT <= 0) {
         MOVE_COUNT = get_new_move_count();
         SPACE_SHIP_DIRECTION = get_new_direction(SPACE_SHIP_DIRECTION);
     }

     // choose a movable direction
     while (contact_edge(
             SPACE_SHIP_DIRECTION,
             INVADER_X_COORD,
             INVADER_Y_COORD,
             invaderHeightPixels,
             invaderWidthPixels
             )) {

         MOVE_COUNT = get_new_move_count();
         SPACE_SHIP_DIRECTION = get_new_direction(SPACE_SHIP_DIRECTION);
     }

     // move image
     move_image(
             SPACE_SHIP_DIRECTION,
             &INVADER_X_COORD,
             &INVADER_Y_COORD,
             invaderHeightPixels,
             invaderWidthPixels
             );

     ALERT_INVADER = true;

     //decrement move count
     MOVE_COUNT --;
		 
		 MARK ++;
	// Clear the interrupt
	TIMER5->ICR |= TIMER_ICR_TATOCINT; 
}
