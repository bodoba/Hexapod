/* *********************************************************************************** */
/*                                                                                     */
/*  Hexapod walking functions                                                          */
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
#include "tripodgait.h"

extern int           ScamperPhase;
extern long          ScamperTracker;
extern unsigned long NextScamperPhaseTime;

#define NUM_TURN_PHASES 6
#define FBSHIFT_TURN    40   // shift front legs back, back legs forward, this much

// for scamper mode
#define SCAMPERPHASES 6
#define KNEEDELAY 35
#define HIPDELAY 100

// for tripod mode
#define NUM_TRIPOD_PHASES 6
#define FBSHIFT    15   // shift front legs back, back legs forward, this much

/* *********************************************************************************** *
 * local prototypes
 * *********************************************************************************** */

void gait_tripod_scamper(int reverse, int turn);
void gait_tripod(int reverse, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod); 
void gait_tripod(int reverse, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod, int leanangle);
void turn(int ccw, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod);
void turn(int ccw, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod, int leanangle);

/* *********************************************************************************** *
 * @brief Process walking commands in Tripod Gait manner
 * *********************************************************************************** */
void walkTripodGait(byte command, byte submode) {
  int direction = 0;
  int factor    = (submode == SUBMODE_2) ? 2 : 1;

  // process commands
  switch (command) {
    case COMMAND_FORWARD:
      direction=1;
    case COMMAND_BACKWARD:
      gait_tripod(direction, (submode==SUBMODE_3)?HIP_BACKWARD_SMALL:HIP_BACKWARD, 
                (submode==SUBMODE_3)?HIP_FORWARD_SMALL:HIP_FORWARD, 
                KNEE_TRIPOD_UP+factor*KNEE_TRIPOD_ADJ, KNEE_DOWN, TRIPOD_CYCLE_TIME*factor);
      break;

    case COMMAND_RIGHT:
      direction=1;
    
    case COMMAND_LEFT:
      turn(direction, (submode==SUBMODE_3)?HIP_BACKWARD_SMALL:HIP_BACKWARD, 
              (submode==SUBMODE_3)?HIP_FORWARD_SMALL:HIP_FORWARD, 
              KNEE_TRIPOD_UP+factor*KNEE_TRIPOD_ADJ, KNEE_DOWN, TRIPOD_CYCLE_TIME*factor);
      break;
  
    case COMMAND_STOMP:
      gait_tripod(1, 90, 90, KNEE_TRIPOD_UP+factor*KNEE_TRIPOD_ADJ, 
                KNEE_DOWN, TRIPOD_CYCLE_TIME*factor);
      break;

    case COMMAND_STAND:
      stand();
      break;
  }
}

/* *********************************************************************************** *
 * @brief this is a tripod gait that tries to go as fast as possible 
 * 
 * It is not waiting for knee motions to complete before beginning the next hip motion 
 * this was experimentally determined and assumes the battery is maintaining +5v to 
 * the servos and they are MG90S or equivalent speed. There is very little room left
 * for slower servo motion. If the battery voltage drops below 6.5V then the BEC may
 * not be able to maintain 5.0V to the servos and they may not complete motions fast
 *  enough for this to work.Process walking commands in Tripod Gait manner
 * *********************************************************************************** */
void gait_tripod_scamper(int reverse, int turn) {

  ScamperTracker += 2;  // for tracking if the user is over-doing it with scamper

  int hipforward, hipbackward;
  
  if (reverse) {
    hipforward = HIP_BACKWARD;
    hipbackward = HIP_FORWARD;
  } else {
    hipforward = HIP_FORWARD;
    hipbackward = HIP_BACKWARD;
  }

  if (millis() >= NextScamperPhaseTime) {
    ScamperPhase++;
    if (ScamperPhase >= SCAMPERPHASES) {
      ScamperPhase = 0;
    }
    switch (ScamperPhase) {
      case 0: NextScamperPhaseTime = millis()+KNEEDELAY; break;
      case 1: NextScamperPhaseTime = millis()+HIPDELAY; break;
      case 2: NextScamperPhaseTime = millis()+KNEEDELAY; break;
      case 3: NextScamperPhaseTime = millis()+KNEEDELAY; break;
      case 4: NextScamperPhaseTime = millis()+HIPDELAY; break;
      case 5: NextScamperPhaseTime = millis()+KNEEDELAY; break;
    }
  }

  //Serial.print("ScamperPhase: "); Serial.println(ScamperPhase);

  transactServos();
  switch (ScamperPhase) {
    case 0:
      // in this phase, center-left and noncenter-right legs raise up at
      // the knee
      setLeg(TRIPOD1_LEGS, NOMOVE, KNEE_SCAMPER, 0);
      setLeg(TRIPOD2_LEGS, NOMOVE, KNEE_DOWN, 0);
      break;

    case 1:
      // in this phase, the center-left and noncenter-right legs move forward
      // at the hips, while the rest of the legs move backward at the hip
      setLeg(TRIPOD1_LEGS, hipforward, NOMOVE, FBSHIFT, turn);
      setLeg(TRIPOD2_LEGS, hipbackward, NOMOVE, FBSHIFT, turn);
      break;

    case 2: 
      // now put the first set of legs back down on the ground
      setLeg(TRIPOD1_LEGS, NOMOVE, KNEE_DOWN, 0);
      setLeg(TRIPOD2_LEGS, NOMOVE, KNEE_DOWN, 0);
      break;

    case 3:
      // lift up the other set of legs at the knee
      setLeg(TRIPOD2_LEGS, NOMOVE, KNEE_SCAMPER, 0, turn);
      setLeg(TRIPOD1_LEGS, NOMOVE, KNEE_DOWN, 0, turn);
      break;
      
    case 4:
      // similar to phase 1, move raised legs forward and lowered legs backward
      setLeg(TRIPOD1_LEGS, hipbackward, NOMOVE, FBSHIFT, turn);
      setLeg(TRIPOD2_LEGS, hipforward, NOMOVE, FBSHIFT, turn);
      break;

    case 5:
      // put the second set of legs down, and the cycle repeats
      setLeg(TRIPOD2_LEGS, NOMOVE, KNEE_DOWN, 0);
      setLeg(TRIPOD1_LEGS, NOMOVE, KNEE_DOWN, 0);
      break;  
  }
  commitServos();
}

/* *********************************************************************************** *
 * @brief walk in tripog gait (this version makes leanangle zero)
 * 
 * The gait consists of 6 phases. This code determines what phase we are currently
 * in by using the millis clock modulo the desired time period that all six phases
 * should consume.
 * 
 * Right now each phase is an equal amount of time but this may not be optimal
 * *********************************************************************************** */
void gait_tripod(int reverse, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod) {
    gait_tripod(reverse, hipforward, hipbackward, kneeup, kneedown, timeperiod, 0);      
}

/* *********************************************************************************** *
 * @brief walk in tripog gait (this version makes leanangle zero)
 * 
 * The gait consists of 6 phases. This code determines what phase we are currently
 * in by using the millis clock modulo the desired time period that all six phases
 * should consume.
 * 
 * Right now each phase is an equal amount of time but this may not be optimal
 * *********************************************************************************** */
void gait_tripod(int reverse, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod, int leanangle) {
  if (reverse) {
    int tmp = hipforward;
    hipforward = hipbackward;
    hipbackward = tmp;
  }
  
  long t = hexmillis()%timeperiod;
  long phase = (NUM_TRIPOD_PHASES*t)/timeperiod;

  transactServos(); // defer leg motions until after checking for crashes
  switch (phase) {
    case 0:
      // in this phase, center-left and noncenter-right legs raise up at
      // the knee
      setLeg(TRIPOD1_LEGS, NOMOVE, kneeup, 0, 0, leanangle);
      break;

    case 1:
      // in this phase, the center-left and noncenter-right legs move forward
      // at the hips, while the rest of the legs move backward at the hip
      setLeg(TRIPOD1_LEGS, hipforward, NOMOVE, FBSHIFT);
      setLeg(TRIPOD2_LEGS, hipbackward, NOMOVE, FBSHIFT);
      break;

    case 2: 
      // now put the first set of legs back down on the ground
      setLeg(TRIPOD1_LEGS, NOMOVE, kneedown, 0, 0, leanangle);
      break;

    case 3:
      // lift up the other set of legs at the knee
      setLeg(TRIPOD2_LEGS, NOMOVE, kneeup, 0, 0, leanangle);
      break;
      
    case 4:
      // similar to phase 1, move raised legs forward and lowered legs backward
      setLeg(TRIPOD1_LEGS, hipbackward, NOMOVE, FBSHIFT);
      setLeg(TRIPOD2_LEGS, hipforward, NOMOVE, FBSHIFT);
      break;

    case 5:
      // put the second set of legs down, and the cycle repeats
      setLeg(TRIPOD2_LEGS, NOMOVE, kneedown, 0, 0, leanangle);
      break;  
  }
  commitServos(); // implement all leg motions
}

void turn(int ccw, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod) {
  turn(ccw, hipforward, hipbackward, kneeup, kneedown, timeperiod, 0);
}

void turn(int ccw, int hipforward, int hipbackward, int kneeup, int kneedown, long timeperiod, int leanangle) {
  // use tripod groups to turn in place
  if (ccw) {
    int tmp = hipforward;
    hipforward = hipbackward;
    hipbackward = tmp;
  }
  
  long t = hexmillis()%timeperiod;
  long phase = (NUM_TURN_PHASES*t)/timeperiod;

  switch (phase) {
    case 0:
      // in this phase, center-left and noncenter-right legs raise up at
      // the knee
      setLeg(TRIPOD1_LEGS, NOMOVE, kneeup, 0);
      break;

    case 1:
      // in this phase, the center-left and noncenter-right legs move clockwise
      // at the hips, while the rest of the legs move CCW at the hip
      setLeg(TRIPOD1_LEGS, hipforward, NOMOVE, FBSHIFT_TURN, 1);
      setLeg(TRIPOD2_LEGS, hipbackward, NOMOVE, FBSHIFT_TURN, 1);
      break;

    case 2: 
      // now put the first set of legs back down on the ground
      setLeg(TRIPOD1_LEGS, NOMOVE, kneedown, 0);
      break;

    case 3:
      // lift up the other set of legs at the knee
      setLeg(TRIPOD2_LEGS, NOMOVE, kneeup, 0);
      break;
      
    case 4:
      // similar to phase 1, move raised legs CW and lowered legs CCW
      setLeg(TRIPOD1_LEGS, hipbackward, NOMOVE, FBSHIFT_TURN, 1);
      setLeg(TRIPOD2_LEGS, hipforward, NOMOVE, FBSHIFT_TURN, 1);
      break;

    case 5:
      // put the second set of legs down, and the cycle repeats
      setLeg(TRIPOD2_LEGS, NOMOVE, kneedown, 0);
      break;  
  } 
}