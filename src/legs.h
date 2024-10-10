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
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#ifndef LEGMOVEMENTS_H
#define LEGMOVEMENTS_H

/* *********************************************************************************** */
/* Servo Settings                                                                      */
/* *********************************************************************************** */

#define SERVO_CENTER      ((SERVO_MAX-SERVO_MIN)/2 + SERVO_MIN)
#define SERVO_FREQ         50  // Analog servos run at ~50 Hz updates
#define OSC_FREQ     26062360  // calculated oscillator frequency
#define NUM_SERVO          12  // 6 Legs by 2 Joints -> 12 Servos
#define NUM_LEGS            6  // 6 legs
#define TRIM_ZERO           0  // this value is the midpoint of the trim range (a byte)
#define TIMEFACTOR         10L
#define SERVO_IIC_ADDR  (0x40) 

/* *********************************************************************************** */
/* Servo Settings                                                                      */
/* *********************************************************************************** */

#define HIP_MIN     140
#define HIP_MAX     400
#define KNEE_MIN    195
#define KNEE_MAX    419

// fake value meaning this aspect of the leg (knee or hip) shouldn't move
#define NOMOVE (-1)   

// Some info about the geomentry...
#define LEFT_START  3 // first leg that is on the left side
#define RIGHT_START 0 // first leg that is on the right side
#define KNEE_OFFSET 6 // add this to a leg number to get the knee servo number

#define ISFRONTLEG(LEG) (LEG==0||LEG==5)
#define ISMIDLEG(LEG)   (LEG==1||LEG==4)
#define ISBACKLEG(LEG)  (LEG==2||LEG==3)
#define ISLEFTLEG(LEG)  (LEG==0||LEG==1||LEG==2)
#define ISRIGHTLEG(LEG) (LEG==3||LEG==4||LEG==5)

extern bool ServosDetached;

/* *********************************************************************************** */
/* Prototypes                                                                          */
/* *********************************************************************************** */

// Initialize Servos
void initServos(void);
void checkForServoSleep(void);

// Set servo Position
void setServo(int servonum, unsigned int position);

// Set leg position
void setLeg(int legmask, int hip_pos, int knee_pos, int adj);
void setLeg(int legmask, int hip_pos, int knee_pos, int adj, int raw);
void setLeg(int legmask, int hip_pos, int knee_pos, int adj, int raw, int leanangle);

// Set hip position
void setHipRaw(int leg, int pos);
void setHip(int leg, int pos);
void setHipRawAdj(int leg, int pos, int adj);

// Set knee position
void setKnee(int leg, int pos);

// help with servo movement
void checkForCrashingHips(void);
void commitServos();
void transactServos();
void detachAllServos();
unsigned long hexmillis();

#endif