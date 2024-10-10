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
#include "led.h"

/* *********************************************************************************** */
/* flash dio state to blink LED                                                       */
/* *********************************************************************************** */
void flashLed(void *dio) {
    static uint8_t state[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int led = (int)dio;
    
    // turn ON LED every 5th cylcle for one cycle
    if ( state[led] == 0 ) {
        digitalWrite(led, 0);
    } else {
        digitalWrite(led, 1);
    }
    
    state[led]++;
    if ( state[led] >= 5 ) {
        state[led] = 0;
    }
}

/* *********************************************************************************** */
/* Toggle LED in different patterns                                                    */
/* *********************************************************************************** */
bool toggleLED(int pin, int mode) {
  typedef struct alarmTimer_t {
    unsigned long alarm;
    bool          status;
  } alarmTimer_t;

  static alarmTimer_t alarmTimer[NUM_PINS];
  unsigned long now = millis();
  
  if ( now > alarmTimer[pin].alarm ) {  
    if ( mode == MODE_ON ) {
      alarmTimer[pin].status = true;
      alarmTimer[pin].alarm  = now+10L;
    
    } else if ( mode == MODE_OFF ) {
      alarmTimer[pin].status = false;
      alarmTimer[pin].alarm  = now+10L;

    } else { 
      switch(mode) {
        case MODE_SLOW_BLINK:
          alarmTimer[pin].alarm  = now+1000L;
          break;
      
        case MODE_FAST_BLINK:
          alarmTimer[pin].alarm  = now+250L;
          break;
      
        case MODE_SLOW_FLASH:
          if (alarmTimer[pin].status) {
            alarmTimer[pin].alarm  = now+10000L;
          } else {
            alarmTimer[pin].alarm  = now+20L;
          }
          break;
      
        case MODE_FAST_FLASH:
          if (alarmTimer[pin].status) {
            alarmTimer[pin].alarm  = now+250L;
          } else {
            alarmTimer[pin].alarm  = now+50L;
          }
          break;

        default:
          break;
      }
      alarmTimer[pin].status = alarmTimer[pin].status ? false : true;
    }
  }

  digitalWrite(pin, alarmTimer[pin].status ? 0 : 1);

  return(alarmTimer[pin].status);
}