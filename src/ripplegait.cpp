/* *********************************************************************************** */
/*                                                                                     */
/*  Hexapod ripple gait walking functions                                              */
/*                                                                                     */
/* *********************************************************************************** */
/*  Big parts of this code are based on the great work the good people at Vorpal       */
/*  Robotics provied in their open source projects.                                    */ 
/*                                                                                     */
/* Fpr more information on their projects, please check out:                           */
/* Main website:                  http://www.vorpalrobotics.com                        */
/* Store (for parts and kits):    http://store.vorpalrobotics.com                      */
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
#include "Arduino.h"
#include "positions.h"
#include "ripplegait.h"
#include "hexabot.h"


/* *********************************************************************************** *
 * local constants
 * *********************************************************************************** */

#define KNEE_RIPPLE_UP (KNEE_NEUTRAL-20)
#define KNEE_RIPPLE_DOWN (KNEE_DOWN)

#define HIP_FORWARD_RIPPLE (HIP_NEUTRAL+HIPSWING_RIPPLE)
#define HIP_BACKWARD_RIPPLE (HIP_NEUTRAL-HIPSWING_RIPPLE)
#define HIPSWING_RIPPLE 25

#define FBSHIFT    15   // shift front legs back, back legs forward, this much

#define RIPPLE_CYCLE_TIME 1000


/* *********************************************************************************** *
 * local prototypes
 * *********************************************************************************** */

void gait_ripple(int turn, int reverse, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod);
void gait_ripple(int turn, int reverse, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod, int leanangle);

/* *********************************************************************************** *
 * @brief Process walking commands in Tripod Gait manner
 * *********************************************************************************** */
void walkRippleGait(byte command) {
    // process commands
    switch (command) {
        case COMMAND_FORWARD:
            gait_ripple(0, 0, HIP_FORWARD_RIPPLE, HIP_BACKWARD_RIPPLE, KNEE_RIPPLE_UP, KNEE_RIPPLE_DOWN, RIPPLE_CYCLE_TIME, 0);
            break;
        
        case COMMAND_BACKWARD:
            gait_ripple(0, 1, HIP_FORWARD_RIPPLE, HIP_BACKWARD_RIPPLE, KNEE_RIPPLE_UP, KNEE_RIPPLE_DOWN, RIPPLE_CYCLE_TIME, 0);
            break;

        case COMMAND_RIGHT:
            gait_ripple(1, 1, HIP_FORWARD_RIPPLE, HIP_BACKWARD_RIPPLE, KNEE_RIPPLE_UP, KNEE_RIPPLE_DOWN, RIPPLE_CYCLE_TIME, 0);
            break;    
    
        case COMMAND_LEFT:
            gait_ripple(1, 0, HIP_FORWARD_RIPPLE, HIP_BACKWARD_RIPPLE, KNEE_RIPPLE_UP, KNEE_RIPPLE_DOWN, RIPPLE_CYCLE_TIME, 0);
            break;
  
        case COMMAND_STOMP:
            gait_ripple(0, 0, HIP_NEUTRAL,        HIP_NEUTRAL,         KNEE_RIPPLE_UP, KNEE_RIPPLE_DOWN, RIPPLE_CYCLE_TIME, 0);
            break;

        case COMMAND_STAND:
            stand();            
            break;
  }
}

/* *********************************************************************************** *
 * @brief walk by lifting only one leg at a time (this version makes leanangle zero)
 * 
 * The gait consists of 19 phases. This code determines what phase we are currently
 * in by using the millis clock modulo the desired time period that all phases should
 * consume. Right now each phase is an equal amount of time but this may not be optimal
 * *********************************************************************************** */
void gait_ripple(int turn, int reverse, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod) {
  gait_ripple(turn, reverse, hipforward, hipbackward, kneeup, kneedown, timeperiod, 0);
}
/* *********************************************************************************** *
 * @brief walk by lifting only one leg at a time
 * 
 * The gait consists of 19 phases. This code determines what phase we are currently
 * in by using the millis clock modulo the desired time period that all phases should
 * consume. Right now each phase is an equal amount of time but this may not be optimal
 * *********************************************************************************** */
void gait_ripple(int turn, int reverse, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod, int leanangle) {
  if (turn) {
    reverse = 1-reverse;  // yeah this is weird but if you're turning you need to reverse the sense of reverse to make left and right turns come out correctly
  }
  if (reverse) {
    int tmp = hipforward;
    hipforward = hipbackward;
    hipbackward = tmp;
  }

#define NUM_RIPPLE_PHASES 19
  
  long t = hexmillis()%timeperiod;
  long phase = (NUM_RIPPLE_PHASES*t)/timeperiod;

  //Serial.print("PHASE: ");
  //Serial.println(phase);

  transactServos();
  
  if (phase == 18) {
    setLeg(ALL_LEGS, hipbackward, NOMOVE, FBSHIFT, turn);
  } else {
    int leg = phase/3;  // this will be a number between 0 and 5 because phase==18 is handled above
    leg = 1<<leg;
    int subphase = phase%3;

    switch (subphase) {
      case 0:
        setLeg(leg, NOMOVE, kneeup, 0);
        break;
      case 1:
        setLeg(leg, hipforward, NOMOVE, FBSHIFT, turn);  // move in "raw" mode if turn is engaged, this makes all legs ripple in the same direction
        break;
      case 2:
        setLeg(leg, NOMOVE, kneedown, 0);
        break;     
    }
  }
  commitServos();
}
