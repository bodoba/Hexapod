/* *********************************************************************************** */
/*                                                                                     */
/*  Firmware for hexapod robot controlled by MQTT messages                             */
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
#include "legs.h"
#include "positions.h"

/* *********************************************************************************** */
/* @brief put bot in a neutral stading position position                               */
/* *********************************************************************************** */
void stand() {
  transactServos();
  setLeg(ALL_LEGS, HIP_NEUTRAL, KNEE_STAND, 0);
  commitServos(); 
}

/* *********************************************************************************** */
/* @brief Used to install servos, sets all servos to 90 degrees                        */
/* *********************************************************************************** */
void stand_90_degrees() {
  transactServos();
  setLeg(ALL_LEGS, 90, 90, 0);
  commitServos();
}

/* *********************************************************************************** */
/* @brief Lay down                                                                     */
/* *********************************************************************************** */
void laydown() {
  setLeg(ALL_LEGS, HIP_NEUTRAL, KNEE_UP, 0);
}

/* *********************************************************************************** */
/* @brief Fold legs up                                                                     */
/* *********************************************************************************** */
void foldup() {
  setLeg(ALL_LEGS, NOMOVE, KNEE_FOLD, 0);
  for (int i = 0; i < NUM_LEGS; i++) 
        setHipRaw(i, HIP_FOLD);
}

/* *********************************************************************************** */
/* @brief Stand on tiptoes                                                             */
/* *********************************************************************************** */
void tiptoes() {
  setLeg(ALL_LEGS, HIP_NEUTRAL, KNEE_TIPTOES, 0);
}

/* *********************************************************************************** */
/* @brief ballet flutter legs on pointe                                                                     */
/* *********************************************************************************** */
void flutter() { 
#define NUM_FLUTTER_PHASES 4
#define FLUTTER_TIME 200
#define KNEE_FLUTTER (KNEE_TIPTOES+20)

  long t = hexmillis()%(FLUTTER_TIME);
  long phase = (NUM_FLUTTER_PHASES*t)/(FLUTTER_TIME);

  setLeg(ALL_LEGS, HIP_NEUTRAL, NOMOVE, 0, 0);
  
  switch (phase) {
    case 0:
      setLeg(TRIPOD1_LEGS, NOMOVE, KNEE_FLUTTER, 0, 0);
      setLeg(TRIPOD2_LEGS, NOMOVE, KNEE_TIPTOES, 0, 0);
      break;
    case 1:
      setLeg(TRIPOD1_LEGS, NOMOVE, KNEE_TIPTOES, 0, 0);
      setLeg(TRIPOD2_LEGS, NOMOVE, KNEE_TIPTOES, 0, 0);
      break;
    case 2:
      setLeg(TRIPOD2_LEGS, NOMOVE, KNEE_FLUTTER, 0, 0);
      setLeg(TRIPOD1_LEGS, NOMOVE, KNEE_TIPTOES, 0, 0);
      break;
    case 3:
      setLeg(TRIPOD2_LEGS, NOMOVE, KNEE_TIPTOES, 0, 0);
      setLeg(TRIPOD1_LEGS, NOMOVE, KNEE_TIPTOES, 0, 0);
      break;
  }
}