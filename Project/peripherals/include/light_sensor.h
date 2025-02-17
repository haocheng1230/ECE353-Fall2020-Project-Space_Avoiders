// Copyright (c) 2014-16, Joe Krachey
// All rights reserved.
//
// Redistribution and use in binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in binary form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#ifndef __LIGHT_SENSOR_H__
#define __LIGHT_SENSOR_H__

#include <stdint.h>
#include <stdbool.h>

#include "driver_defines.h"
#include "boardUtil.h"

//*****************************************************************************
// Used to initialize the light sensor circuitry
//
// Configuration Info
//		Fill out relevant information in boardUtil.h.  boardUtil.h defines 
//		how various peripherals are physically connected to the board.
//*****************************************************************************
void light_sensor_initialize(void);

//*****************************************************************************
// Returns the most current digital reading of the light sensor  
//*****************************************************************************
bool light_sensor_get_digital(void);

//*****************************************************************************
// Returns the most current reading of the light sensor.  Only the lower 12-bits
// contain data.
//*****************************************************************************
uint16_t light_sensor_get_analog(void);

//*****************************************************************************
// Runs a test on the light sensor
//*****************************************************************************
void light_sensor_test(void);


#endif
