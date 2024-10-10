/* *********************************************************************************** */
/*                                                                                     */
/*  Hexapod quadruped gait walking functions                                              */
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
#include "quadgait.h"
#include "hexabot.h"

#define FBSHIFT_QUAD 25
#define HIP_FORWARD_QUAD (HIP_FORWARD)
#define HIP_BACKWARD_QUAD (HIP_BACKWARD)
#define KNEE_QUAD_UP (KNEE_DOWN+30)
#define KNEE_QUAD_DOWN (KNEE_DOWN)
#define QUAD_CYCLE_TIME 600
#define NUM_QUAD_PHASES 6

/* *********************************************************************************** *
 * local prototypes
 * *********************************************************************************** */
void gait_quad(int turn, int reverse, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod, int leanangle);

/* *********************************************************************************** *
 * @brief Process walking commands in Quadruple Gait manner
 * *********************************************************************************** */
void walkQuadGait(byte command) {
    switch (command) {
      case COMMAND_FORWARD:
        gait_quad(0, 0, HIP_FORWARD_QUAD, HIP_BACKWARD_QUAD, KNEE_QUAD_UP, KNEE_QUAD_DOWN, QUAD_CYCLE_TIME, 0);
        break;
      case COMMAND_BACKWARD:
        gait_quad(0, 1, HIP_FORWARD_QUAD, HIP_BACKWARD_QUAD, KNEE_QUAD_UP, KNEE_QUAD_DOWN, QUAD_CYCLE_TIME, 0);
        break;
      case COMMAND_LEFT:
        gait_quad(1, 0, HIP_FORWARD_QUAD, HIP_BACKWARD_QUAD, KNEE_QUAD_UP, KNEE_QUAD_DOWN, QUAD_CYCLE_TIME, 0);
        break;
      case COMMAND_RIGHT:
        gait_quad(1, 1, HIP_FORWARD_QUAD, HIP_BACKWARD_QUAD, KNEE_QUAD_UP, KNEE_QUAD_DOWN, QUAD_CYCLE_TIME, 0);
        break;
      case COMMAND_STOMP:
        gait_quad(1, 1, HIP_NEUTRAL, HIP_NEUTRAL, KNEE_QUAD_UP, KNEE_QUAD_DOWN, QUAD_CYCLE_TIME, 0);
        break;
      case COMMAND_STAND:
        gait_quad(1, 1, HIP_NEUTRAL, HIP_NEUTRAL, KNEE_QUAD_DOWN, KNEE_QUAD_DOWN, QUAD_CYCLE_TIME, 0);
        break;
    }
}

/* *********************************************************************************** *
 * @brief walk with middle legs raied up 
 *
 * The gait walks using a quadruped gait with middle legs raised up. This code 
 * determines what phase we are currently in by using the millis clock modulo the 
 * desired time period that all phases should consume. Right now each phase is an 
 * equal amount of time but this may not be optimalProcess walking commands in 
 * Tripod Gait manner
 * *********************************************************************************** */
void gait_quad(int turn, int reverse, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod, int leanangle) {

    if (turn) {
        reverse = 1-reverse;  // yeah this is weird but if you're turning you need to reverse the sense of reverse to make left and right turns come out correctly
     }

    if (reverse) {
        int tmp = hipforward;
        hipforward = hipbackward;
        hipbackward = tmp;
    }

    long t = hexmillis()%timeperiod;
    long phase = (NUM_QUAD_PHASES*t)/timeperiod;

    transactServos();
    setLeg(MIDDLE_LEGS, HIP_NEUTRAL, KNEE_UP_MAX, FBSHIFT_QUAD, 0);
  
    switch (phase) {
        case 0:
            // in this phase, center-left and noncenter-right legs raise up at
            // the knee
            setLeg(QUAD1_LEGS, NOMOVE, kneeup, FBSHIFT_QUAD, turn);
            // use the middle legs to try to counter balance
            if (kneeup != kneedown) { // if not standing still
                setLeg(MIDDLE_LEGS, reverse?HIP_BACKWARD_MAX:HIP_FORWARD_MAX, NOMOVE, 0, 1);
            }
            break;

        case 1:
            // in this phase, the center-left and noncenter-right legs move forward
            // at the hips, while the rest of the legs move backward at the hip
            setLeg(QUAD1_LEGS, hipforward, NOMOVE, FBSHIFT_QUAD, turn);
            setLeg(QUAD2_LEGS, hipbackward, NOMOVE, FBSHIFT_QUAD, turn);
            break;

        case 2: 
            // now put the first set of legs back down on the ground
            setLeg(QUAD1_LEGS, NOMOVE, kneedown, 0, turn);
            break;

        case 3:
            // lift up the other set of legs at the knee
            setLeg(QUAD2_LEGS, NOMOVE, kneeup, 0, turn);
            if (kneeup != kneedown) {
                setLeg(MIDDLE_LEGS, reverse?HIP_FORWARD_MAX:HIP_BACKWARD_MAX, NOMOVE, 0, 1);
            }
            break;
      
        case 4:
            // similar to phase 1, move raised legs forward and lowered legs backward
            setLeg(QUAD1_LEGS, hipbackward, NOMOVE, FBSHIFT_QUAD, turn);
            setLeg(QUAD2_LEGS, hipforward, NOMOVE, FBSHIFT_QUAD, turn);
            break;

        case 5:
            // put the second set of legs down, and the cycle repeats
            setLeg(QUAD2_LEGS, NOMOVE, kneedown, 0, turn);
            break;  
    }
    commitServos();
}