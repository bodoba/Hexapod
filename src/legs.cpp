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
#include "serial.h"
#include "legs.h"

byte TrimInEffect    = 0;
byte deferServoSet   = 0;
bool ServosDetached  = true;

typedef struct servo_t {
  byte           pin;

  unsigned short ServoPos;     // the last commanded position of each servo
  unsigned short ServoTarget;
  long           ServoTime;    // the time that each servo was last commanded to a new position
  byte           ServoTrim;    // trim values for fine adjustments to servo horn positions
} servo_t;

servo_t Servo[NUM_SERVO] = {
  {  0,  0,  0, 0L, 0   },  // Leg 0 - Hipp
  {  1,  0,  0, 0L, 0   },  // Leg 1 - Hipp
  {  2,  0,  0, 0L, 0   },  // Leg 2 - Hipp
  {  3,  0,  0, 0L, 0   },  // Leg 3 - Hipp
  {  4,  0,  0, 0L, 0   },  // Leg 4 - Hipp
  {  5,  0,  0, 0L, 0   },  // Leg 5 - Hipp
  {  8,  0,  0, 0L, 0   },  // Leg 0 - Knee
  {  9,  0,  0, 0L, 0   },  // Leg 1 - Knee   
  { 10,  0,  0, 0L, 0   },  // Leg 2 - Knee   
  { 11,  0,  0, 0L, 0   },  // Leg 3 - Knee   
  { 12,  0,  0, 0L, 0   },  // Leg 4 - Knee   
  { 13,  0,  0, 0L, 0   },  // Leg 5 - Knee   
};

Adafruit_PWMServoDriver servoDriver = Adafruit_PWMServoDriver(SERVO_IIC_ADDR, Wire);

void checkForCrashingHips(void);


/* *********************************************************************************** */
/* @brief Initialize Servos                                                            */
/* *********************************************************************************** */
void initServos(void) {
  servoDriver.begin();                              // Start Servo Controller board
  servoDriver.setOscillatorFrequency(OSC_FREQ);     
  servoDriver.setPWMFreq(SERVO_FREQ);               // Analog servos run at ~50 Hz updates
}

/* *********************************************************************************** */
/* @brief Lowest level function for setting servo positions                   */
/* *********************************************************************************** */
void setServo(int servonum, unsigned int position) {
  servonum = constrain(servonum,0,15);
  position = constrain(position,0,180);

  if (servonum < 12 ) { 
    // shift both the hips and legs independently in cycles 6 long
    int tmp = (servonum)%6; 
    if (servonum>5) { // if it was a hip, make it still be a hip (the mod 6 above would have made it too low)
      tmp += 6;   
    }
    servonum = constrain(tmp, 0, 11);
  }
  
  if (position != Servo[servonum].ServoPos) {
    Servo[servonum].ServoTime = millis();
  }
  Servo[servonum].ServoPos = position;  // keep data on where the servo was last commanded to go
  
  int p = 0; 
  if (servonum<6) {                                    // hip
    p = map(position,0,180,HIP_MIN,HIP_MAX);  
  } else if ( servonum < 12 ) {                        // knee
    p = map(position,180,0,KNEE_MIN,KNEE_MAX);
  }
  
  if (TrimInEffect && servonum < 12) {
    p += Servo[servonum].ServoTrim - TRIM_ZERO; // adjust microseconds by trim value which is renormalized to the range -127 to 128    
  }

  //sprintf(msg, "setServo(servonum: %d, position: %d ) => setPwm( pin: %d, on: 0, off: %d )", servonum, position, Servo[servonum].pin, p);
  //mqttSendMessage("/%s/Debug", msg );

  if (!deferServoSet) {
    servoDriver.setPWM(Servo[servonum].pin, 0, p);
    ServosDetached = false;
  }
}

/* *********************************************************************************** */
/* @brief Allow to set servo positions and delay actual movement                       */
/* *********************************************************************************** */
void transactServos() {
  deferServoSet = 1;
}

/* *********************************************************************************** */
/* @brief Move all servos to designated position                                       */
/* *********************************************************************************** */
void commitServos() {
  checkForCrashingHips();
  deferServoSet = 0;
  for (int servo = 0; servo < 2*NUM_LEGS; servo++) {
    setServo(servo, Servo[servo].ServoPos);
  }
}
/* *********************************************************************************** */
/* @brief millis that takes into account hexapod size for leg timings                  */
/* *********************************************************************************** */
unsigned long hexmillis() {  
  unsigned long m = (millis() * TIMEFACTOR)/10L;
  return m;
}

/* *********************************************************************************** */
/* @brief sets the positions of both the knee and hip in a single command              */
/* *********************************************************************************** */
void setLeg(int legmask, int hip_pos, int knee_pos, int adj) {
  setLeg(legmask, hip_pos, knee_pos, adj, 0, 0);  // use the non-raw version with leanangle=0
}

/* *********************************************************************************** */
/* @brief sets the positions of both the knee and hip in a single command              */
/* *********************************************************************************** */
void setLeg(int legmask, int hip_pos, int knee_pos, int adj, int raw) {
  setLeg(legmask, hip_pos, knee_pos, adj, raw, 0);
}

/* *********************************************************************************** */
/* @brief This version of setHip does no processing at all (for example to distinguish */
/*        left from right sides)                                                       */
/* *********************************************************************************** */
void setHipRaw(int leg, int pos) {
  setServo(leg, pos);
}

/* *********************************************************************************** */
/* @brief  this version of setHip adjusts for left and right legs so that 0 degrees    */
/*         moves "forward" i.e. toward legs 5-0 which is nominally the front of the    */
/*         robot                                                                       */
/* *********************************************************************************** */
void setHip(int leg, int pos) {
  // reverse the left side for consistent forward motion
  if (leg >= LEFT_START) {
    pos = 180 - pos;
  }
  setHipRaw(leg, pos);
}

/* *********************************************************************************** */
/* @brief This version of setHip adjusts not only for left and right, but also shifts  */
/*        the front legs a little back and the back legs forward to make a better      */
/*        balance for certain gaits like tripod or quadruped                           */
/* *********************************************************************************** */
void setHip(int leg, int pos, int adj) {
  if (ISFRONTLEG(leg)) {
    pos -= adj;
  } else if (ISBACKLEG(leg)) {
    pos += adj;
  }
  // reverse the left side for consistent forward motion
  if (leg >= LEFT_START) {
    pos = 180 - pos;
  }
  setHipRaw(leg, pos);
}

/* *********************************************************************************** */
/* @brief This version of setHip doesn't do mirror images like raw, but it does honor  */ 
/*        the adjust parameter to shift the front/back legs                            */
/* *********************************************************************************** */
void setHipRawAdj(int leg, int pos, int adj) {
  if (leg == 5 || leg == 2) {
    pos += adj;
  } else if (leg == 0 || leg == 3) {
    pos -= adj;
  }

  setHipRaw(leg, pos);
}

/* *********************************************************************************** */
/* @brief Set knee position                                                            */ 
/* *********************************************************************************** */
void setKnee(int leg, int pos) {
  // find the knee associated with leg if this is not already a knee
  if (leg < KNEE_OFFSET) {
    leg += KNEE_OFFSET;
  }
  setServo(leg, pos);
}

/* *********************************************************************************** */
/* @brief sets the positions of both the knee and hip in a single command              */
/* *********************************************************************************** */
void setLeg(int legmask, int hip_pos, int knee_pos, int adj, int raw, int leanangle) {
  for (int i = 0; i < NUM_LEGS; i++) {
    if (legmask & 0b1) {  // if the lowest bit is ON then we are moving this leg
      if (hip_pos != NOMOVE) {
        if (!raw) {
          setHip(i, hip_pos, adj);
        } else {
          setHipRaw(i, hip_pos);
        }
      }

      if (knee_pos != NOMOVE) {
        int pos = knee_pos;
        if (leanangle != 0) {
          switch (i) {
            case 0: case 6: case 5: case 11:
              if (leanangle < 0) pos -= leanangle;
              break;
            case 1: case 7: case 4: case 10:
              pos += abs(leanangle/2);
              break;
            case 2: case 8: case 3: case 9:
              if (leanangle > 0) pos += leanangle;
              break;
          }
        }
        setKnee(i, pos);
      }
    }
    legmask = (legmask>>1);  // shift down one bit position to check the next legmask bit
  }
}

/* *********************************************************************************** *
 * @brief Takes a look at the leg angles and try to figure out if the commanded
 *        positions might cause servo stall.  
 *
 * Now the correct way to do this would be to do fairly extensive trig computations to 
 * see if the edges of the hips touch. However, Arduino isn't really set up to do 
 * complicated trig stuff. It would take a lot of code space and a lot of time. So
 * we're just using a simple approximation. In practice it stops very hard stall 
 * situations. Very minor stalls (where the motor is commanded a few degress farther
 * than it can physically go) may still occur, but those won't draw much power (the
 * current draw is proportional to how far off the mark the servo is).
 * *********************************************************************************** */
void checkForCrashingHips(void) {
  for (int leg = 0; leg < NUM_LEGS; leg++) {
    if (Servo[leg].ServoPos > 85) {
      continue; // it's not possible to crash into the next leg in line unless the angle is 85 or less
    }
    int nextleg = ((leg+1)%NUM_LEGS);
    if (Servo[nextleg].ServoPos < 100) {
      continue;   // it's not possible for there to be a crash if the next leg is less than 100 degrees
                  // there is a slight assymmetry due to the way the servo shafts are positioned, that's why
                  // this number does not match the 85 number above
    }
    int diff = Servo[nextleg].ServoPos - Servo[leg].ServoPos;
    // There's a fairly linear relationship
    if (diff <= 85) {
      // if the difference between the two leg positions is less than about 85 then there
      // is not going to be a crash (or maybe just a slight touch that won't really cause issues)
      continue;
    }
    // if we get here then the legs are touching, we will adjust them so that the difference is less than 85
    int adjust = (diff-85)/2 + 1;  // each leg will get adjusted half the amount needed to avoid the crash
    
    // to debug crash detection, make the following line #if 1, else make it #if 0
    PRINT("#CRASH:");
    PRINT(leg);PRINT("="); PRINT(Servo[leg].ServoPos);
    PRINT("/");PRINT(nextleg);PRINT("="); PRINT(Servo[nextleg].ServoPos);
    PRINT(" Diff=");PRINT(diff); PRINT(" ADJ=");PRINTLN(adjust);

    setServo(leg, Servo[leg].ServoPos + adjust);   
    setServo(nextleg, Servo[nextleg].ServoPos - adjust);
  }
}

/* *********************************************************************************** *
 * @brief See if the servo driver module went to sleep and wake it up again
 *
 * Short power dips can cause the servo driver to put itself to sleep the 
 * checkForServoSleep() function uses IIC protocol to ask the servo driver if
 * it's asleep. If it is, this function wakes it back up. You'll see the robot
 * stutter step for about half a second and a chirp is output to indicate what happened. 
 * This happens more often on low battery conditions. When the battery gets low enough, 
 * however, this code will not be able to wake it up again. If your robot constantly
 * resets even though the battery is fully charged, you may have too much friction
 * on the leg hinges, or you may have a bad servo that's drawing more power than usual.
 * A bad BEC can also cause the issue.
 * *********************************************************************************** */

unsigned long freqWatchDog = 0;
unsigned long SuppressScamperUntil = 0;  // if we had to wake up the servos, suppress the power hunger scamper mode for a while

void checkForServoSleep(void) {
  if (millis() > freqWatchDog) { // do NOT use hexmillis here, we want real time

    // See if the servo driver module went to sleep, probably due to a short power dip
    Wire.beginTransmission(SERVO_IIC_ADDR);
    Wire.write(0);  // address 0 is the MODE1 location of the servo driver, see documentation on the PCA9685 chip for more info
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)SERVO_IIC_ADDR, (uint8_t)1);
    int mode1 = Wire.read();
    if (mode1 & 16) { // the fifth bit up from the bottom is 1 if controller was asleep
      // wake it up!
      initServos();
      SuppressScamperUntil = millis() + 10000;  // no scamper for you! (for 10 seconds because we ran out of power, give the battery
                                                // a bit of time for charge migration and let the servos cool down). Don't use hexmillis here.
    }
    freqWatchDog = millis() + 100; // do NOT use hexmillis here
  }
}

/* *********************************************************************************** *
 * @brief Save enery by detaching all servos  
 * *********************************************************************************** */
void detachAllServos() {
  //Serial.println("DETACH");
  for (int i = 0; i < 16; i++) {
    servoDriver.setPin(i,0,false); // stop pulses which will quickly detach the servo
  }
  ServosDetached = true;
}