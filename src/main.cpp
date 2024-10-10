/* *********************************************************************************** */
/*                                                                                     */
/*  Firmware for hexapod robot controlled by MQTT messages                             */
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

#include "wifi.h"
#include "mqtt.h"
#include "persistence.h"
#include "serial.h"
#include "led.h"

#include "hexabot.h"
#include "legs.h"
#include "positions.h"
#include "tripodgait.h"
#include "ripplegait.h"
#include "quadgait.h"
#include "wave.h"

#define COMMAND_TIMEOUT (3000L)     // Time out commands after 3 seconds
#define ENERGYSAVER     (10000L)    // Detach Servos after 10s standing still

byte botMode      = MODE_WALK; 
byte botSubmode   = SUBMODE_1;
byte botCommand   = COMMAND_NONE;
unsigned long botCommandUpdate = 0L;       

unsigned long lastMovement = 0L;      // Time the last movemeent command was executed

int  factor       = 1;
int  ScamperPhase = 0;
unsigned long NextScamperPhaseTime = 0;
long ScamperTracker = 0;

char msg[512];  // all purpose message buffer

// System services

void heartbeat(void*);
void lights(void*);

void resetLastMovement(void);


// MQTT support
void  mqttCbCmd(const char*);
void  mqttCbStatus(const char*);
char* parseCommand(char* cmd);

/* *********************************************************************************** */
/* Custom Types                                                                        */
/* *********************************************************************************** */
typedef struct {                 // Tasks to be regulary scheduled
    unsigned long last_time;     // time of last call
    unsigned long interval;      // cycle time to run task
    void (*task)(void*);         // pointer to task handler
    void *argument;              // call argument
} task_t;

/* *********************************************************************************** */
/* MQTT Services                                                                       */
/* *********************************************************************************** */
service_t mqttServiceList[] = {
    {"Cmd",    mqttCbCmd},
    {"Status", mqttCbStatus},
    {NULL, NULL}
};

/* *********************************************************************************** */
/* @brief MQTT Callback: Return Bot Status                                             */
/* *********************************************************************************** */
void mqttCbStatus(const char *payload) {
  mqttSendMessage("/%s/Error", "Status report not implemented yet");
}

/* *********************************************************************************** */
/* @brief MQTT Callback: Process command                                               */
/* *********************************************************************************** */
void mqttCbCmd(const char *payload) {
  char *cmd = strdup(payload);
  char* cursor = parseCommand(cmd);
  
  if (!strcmp(cmd,"SetLeg")) {                               // Move single leg: #Leg °Hip °Knee
    botCommand = COMMAND_NONE;
    
    char *strLeg=cursor;
    char *strHip=parseCommand(strLeg);
    char *strKnee=parseCommand(strHip);
    int leg=1<<atoi(strLeg);
    int knee=atoi(strKnee);
    int hip=atoi(strHip);
    setLeg(leg, hip, knee, 0);

  } else if (!strcmp(cmd,"SetMode")) {                       // Set movement mode 
    char *strMode=cursor;
    if ( !strcmp(strMode,        "Walk")) {
      botMode = MODE_WALK;
    } else if ( !strcmp(strMode, "Ripple")) {
      botMode = MODE_RIPPLE;
    } else if ( !strcmp(strMode, "Quad")) {
      botMode = MODE_QUAD;
    } else if ( !strcmp(strMode, "Wave")) {
      botMode = MODE_WAVE;
    } 

  } else if (!strcmp(cmd,"SetSubMode")) {                   // Set submode 
    char *strMode=cursor;
    switch( strMode[0] ) {
      case '1':
        botSubmode = SUBMODE_1;
        break;
      case '2':
        botSubmode = SUBMODE_2;
        break;
      case '3':
        botSubmode = SUBMODE_3;
        break;
      case '4':
        botSubmode = SUBMODE_4;
        break;
      default:
        botSubmode = SUBMODE_1;
        break;
    }
  } else if (!strcmp(cmd,"Stand90Degrees")) {                // Predefined "Stand 90 Degrees" 
    botCommand = COMMAND_NONE;
    stand_90_degrees();
    resetLastMovement();

  } else if (!strcmp(cmd,"LayDown")) {                       // Predefined "Lay Down" 
    botCommand = COMMAND_NONE;
    laydown();
    resetLastMovement();
  
  } else if (!strcmp(cmd,"TipToes")) {                       // Predefined "Tip toes" 
    botCommand = COMMAND_NONE;
    tiptoes();
    resetLastMovement();
  
  } else if (!strcmp(cmd,"FoldUp")) {                       // Predefined "Fold Up" 
    botCommand = COMMAND_NONE;
    foldup();
    resetLastMovement();

  } else if (!strcmp(cmd,"Forward")) {                      // Move forward 
    botCommand = COMMAND_FORWARD;
    botCommandUpdate = millis();

  } else if (!strcmp(cmd,"Backward")) {                     // Move backward 
    botCommand = COMMAND_BACKWARD;
    botCommandUpdate = millis();

  } else if (!strcmp(cmd,"Right")) {                        // Move right 
    botCommand = COMMAND_RIGHT;
    botCommandUpdate = millis();

  } else if (!strcmp(cmd,"Left")) {                         // Move left 
    botCommand = COMMAND_LEFT;
    botCommandUpdate = millis();

  } else if (!strcmp(cmd,"Stand")) {                        // Stand 
    botCommand = COMMAND_STAND;
    botCommandUpdate = millis();

  } else if (!strcmp(cmd,"Stomp")) {                        // Stomp in Place 
    botCommand = COMMAND_STOMP;
    botCommandUpdate = millis();

  } else if (!strcmp(cmd,"Detach")) {                       // Detach servos 
    botCommand=COMMAND_NONE;
    detachAllServos();

  } else {
    sprintf(msg, "The command %s is not implemented yet", cmd);
    mqttSendMessage("/%s/Error", msg );
  }

  free(cmd);
}

/* *********************************************************************************** */
/* @brief terminate first element in command string and return the next one            */
/* *********************************************************************************** */
char* parseCommand(char* cmd) {
  char *cursor = cmd;

  while (*cursor && *cursor != ' ') cursor++;
  *cursor++ = (char)0; 
  return (cursor);
}

/* *********************************************************************************** */
/* @brief Control LEDs                                                                 */
/* *********************************************************************************** */
void lights(void*) {
  toggleLED( LED_BUILTIN, MODE_FAST_FLASH);
}

/* *********************************************************************************** *
 * @brief Send alive status message                                                    *
 * *********************************************************************************** */
void heartbeat(void*) {
  static unsigned long aliveCounter=0;
  
  sprintf(msg, "#%08ld mode: %c submode: %c command: %c", aliveCounter, botMode, botSubmode, botCommand);
  mqttSendMessage("/%s/Status", msg);

  aliveCounter++;
}

/* *********************************************************************************** *
 * @brief Initialize the System                                                        *
 * *********************************************************************************** */
void setup() {

  // start serial console
  initSerial();

  // load persistent data from EEPROM
  loadFromEEPROM();
  
  // initialize netowrk subsystems
  initWifi();                                              // connect to WiFi network
  initOTA();                                               // allow OTA updates
  initMQTT();                                              // connect to MQTT broker

  // some HW setup
  pinMode(LED_BUILTIN, OUTPUT);                            // use on-board LED
  digitalWrite(LED_BUILTIN, 0);                            // turn LED on
  Wire.begin();                                            // Start I2C bus
  initServos();                                            // inittalize servo handling

  mqttSendMessage("/%s/Status", "Boot sequence complete"); // let the word know that
                                                           // we are ready for business

  stand();                                                 // assume standing position
}


/* *********************************************************************************** */
/* @brief Schedule Table                                                               */
/* *********************************************************************************** */
task_t taskTable[] = {
    //   last run,  interval, task,       argument
    {0L,     500L, lights,              NULL},      // LED effects
    {0L,    1000L, heartbeat,           NULL},      // sent heartbeat message ecery 10s
    {0, 0, NULL, NULL},                             // end marker
};

/* *********************************************************************************** *
 * @brief Reste timestamp of last movement to avoid timeout of servo power             
 * *********************************************************************************** */
void resetLastMovement(void) {
  lastMovement   = millis();
  ServosDetached = false;
}

/* *********************************************************************************** */
/* @brief From Her to Eternity                                                         */
/* *********************************************************************************** */
void loop() {
  unsigned long current_time = millis();

  // some useful checks
  checkForServoSleep();
  checkForCrashingHips();

  // process MQTT communication
  client.loop();

  // check if the is an OTA update request
  ArduinoOTA.handle();

  // let commands time out
  if ( botCommandUpdate < current_time - COMMAND_TIMEOUT ) {
    botCommand = COMMAND_NONE;
  }

  // process commands dependent on mode
  if ( botCommand != COMMAND_NONE ) {
    switch(botMode) {
      case MODE_WALK:
        walkTripodGait(botCommand, botSubmode);
        break;

      case MODE_RIPPLE:
        walkRippleGait(botCommand);
        break;
    
      case MODE_QUAD:
        walkQuadGait(botCommand);
        break;

      case MODE_WAVE:
        wave(botCommand);
        break;

    }
    resetLastMovement();
  }
 
  if ( ServosDetached == false && ( (millis() - lastMovement) > ENERGYSAVER) ) {
    detachAllServos();
    mqttDebug("Servos Detached");
  }
 
  // process tasks table
  for (int index = 0; taskTable[index].task != NULL; index++) {
    if (current_time - taskTable[index].last_time >= taskTable[index].interval) {
      // run task
      (taskTable[index].task)(taskTable[index].argument);
                
      // remember last time this task has been run
      taskTable[index].last_time = current_time;
    }
  }  
}