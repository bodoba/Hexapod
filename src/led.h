/* *********************************************************************************** */
/*                                                                                     */
/*  Convenience routine for LED blinking patterns                                      */
/*                                                                                     */
/* *********************************************************************************** */
/*                                                                                     */
/*  Copyright 2024 by Bodo Bauer <bb@bb-zone.com>                                      */
/*                                                                                     */
/*  This program is free software: you can redistribute it and/or modify               */
/*  it under the terms of the GNU General Public License as published by               */
/*  the Free Software Foundation, either version 3 of the License, or                  */
/*  (at your option) any later version.                                                */
/*                                                                                     */
/*  This program is distributed in the hope that it will be useful,                    */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of                     */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                      */
/*  GNU General Public License for more details.                                       */
/*                                                                                     */
/*  You should have received a copy of the GNU General Public License                  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.              */
/* *********************************************************************************** */
#include <Arduino.h>

#ifndef LED_H
#define LED_H

/* *********************************************************************************** */
/* D1 Mini Pins                                                                        */
/* *********************************************************************************** */
//            PIN     GPIO  // Board Info
#define D1MINI_D0      16   // IO                             //
#define D1MINI_D1       5   // IO, SCL                        // I2C
#define D1MINI_D2       4   // IO, SDA                        // I2C
#define D1MINI_D3       0   // IO, 10k Pull-up                //
#define D1MINI_D4       2   // IO, 10k Pull-up, BUILTIN_LED   // LED
#define D1MINI_D5      14   // IO, SCK                        // One Wire
#define D1MINI_D6      12   // IO, MISO                       // Rain Drop Sensor
#define D1MINI_D7      13   // IO, MOSI                       //
#define D1MINI_D8      15   // IO, 10k Pull-down, SS          //

#define NUM_PINS       17   // 0 - 16 

/* *********************************************************************************** */
/* LED pattern                                                                         */
/* *********************************************************************************** */

#define MODE_ON              1
#define MODE_OFF             2
#define MODE_SLOW_BLINK      3
#define MODE_FAST_BLINK      4
#define MODE_SLOW_FLASH      5
#define MODE_FAST_FLASH      6

/* *********************************************************************************** */
/* Prototypes                                                                          */
/* *********************************************************************************** */

void flashLed(void *dio);                 // turn on dio every 5th cycle for one cycle
bool toggleLED(int pin, int mode);        // flash LED in selected pattern (call in loop)

#endif