#ifndef __SCOREBOARD_H__
#define __SCOREBOARD_H__

#include "eeprom.h"
void eeprom_score_write(uint32_t highscore);
uint32_t eeprom_score_read();
void eeprom_score_reset();


#endif
