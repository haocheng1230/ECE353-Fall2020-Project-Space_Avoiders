#include "hw3_interrupts.h"

static volatile uint16_t PS2_X_DATA = 0;
static volatile uint16_t PS2_Y_DATA = 0;
static volatile PS2_DIR_t PS2_DIR = PS2_DIR_CENTER;

// new added globals
static volatile uint16_t MOVE_COUNT = 0;
static volatile PS2_DIR_t SPACE_SHIP_DIRECTION = PS2_DIR_CENTER;

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

//*****************************************************************************
// TIMER2 ISR is used to determine when to move the Invader
// Trigger every 20ms, priority 0
// The ISR will determine if the Invader
// image can be moved by 1 pixel in the current direction
// by calling the function contact_edge().

// If Invader can be moved, it will call the function move_image()
// to updated the global
// variables INVADER_X_COORD and INVADER_Y_COORD
// that hold the current location of the Invader.
//*****************************************************************************
void TIMER2A_Handler(void)
{

    // move invader if not contacting edge
    if (!contact_edge(PS2_DIR,
            INVADER_X_COORD,
            INVADER_Y_COORD,
            invaderHeightPixels,
            invaderWidthPixels
            )) {
        ALERT_INVADER = true;
    }

    // if the invader can be moved, then update its coord
    if (ALERT_INVADER) {
            move_image(
                    PS2_DIR,
                    &INVADER_X_COORD,
                    &INVADER_Y_COORD,
                    invaderHeightPixels,
                    invaderWidthPixels
                    );
    }
    // Clear the interrupt
	TIMER2->ICR |= TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// TIMER3 ISR is used to determine when to move the spaceship
// Trigger every 10ms, priority 1
//*****************************************************************************
void TIMER3A_Handler(void)
{
     // update if out of moves
     if (MOVE_COUNT <= 0) {
         MOVE_COUNT = get_new_move_count();
         SPACE_SHIP_DIRECTION = get_new_direction(SPACE_SHIP_DIRECTION);
     }

     // choose a movable direction
     while (contact_edge(
             SPACE_SHIP_DIRECTION,
             SHIP_X_COORD,
             SHIP_Y_COORD,
             space_shipHeightPixels,
             space_shipWidthPixels
             )) {

         MOVE_COUNT = get_new_move_count();
         SPACE_SHIP_DIRECTION = get_new_direction(SPACE_SHIP_DIRECTION);
     }

     // move image
     move_image(
             SPACE_SHIP_DIRECTION,
             &SHIP_X_COORD,
             &SHIP_Y_COORD,
             space_shipHeightPixels,
             space_shipWidthPixels
             );

     ALERT_SPACE_SHIP = true;

     //decrement move count
     MOVE_COUNT --;

	// Clear the interrupt
	TIMER3->ICR |= TIMER_ICR_TATOCINT;  
}

//*****************************************************************************
// TIMER4 ISR is used to trigger the ADC
// Trigger every 1ms, priority 2
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
    // Clear the interrupt
    ADC0->ISC |= ADC_ISC_IN2;
}
