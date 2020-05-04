#include "scoreboard.h"

#define ADDR_START    256
#define NUM_BYTES      20

void eeprom_score_write(uint32_t highscore) {
    i2c_status_t status;
    uint16_t addr = 256;
    uint8_t score[4];
    int i = 0;
    score[0] = highscore >> 24;
    score[1] = highscore >> 16;
    score[2] = highscore >> 8;
    score[3] = highscore;
    for (i = 0 ; i < 4 ; i++) {
       status = eeprom_byte_write(I2C1_BASE, addr, score[i]);
    }
}

uint32_t eeprom_score_read() {
    i2c_status_t status;
    uint32_t highscore;
    uint16_t addr = 256;
    uint8_t score[4];
    int i = 0;
    for (i = 0 ; i < 4 ; i++) {
       status = eeprom_byte_read(I2C1_BASE, addr, &score[i]);
    }
    highscore = score[0] << 24 | score[1] << 16 | score[2] << 8 | score[3];
}

void eeprom_score_reset() {
    i2c_status_t status;
		int i;
    uint16_t addr = 256;
    for (i = 0 ; i < 4 ; i++) {
       status = eeprom_byte_write(I2C1_BASE, addr, 0x00);
    }
}