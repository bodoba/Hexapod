/* *********************************************************************************** */
/*                                                                                     */
/*  Provide persistent storage                                                         */
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
#include <EEPROM.h>

#ifndef PERSISTENCE_H
#define PERSISTENCE_H

/* *********************************************************************************** */
/* Size of persistent sotrage                                                          */
/* *********************************************************************************** */
#define EEPROM_SIZE 1024
#ifndef EEPROM_MAGIC
#define EEPROM_MAGIC "BBIOT-0001"
#endif

/* *********************************************************************************** */
/* Custom Types                                                                        */
/* *********************************************************************************** */
typedef struct {                 // persistent data
    char  magic[16];             // used to detect if EEPROM hast vali data
    char  mqtt_broker[40];       // address and port of MQQT broker
    int   mqtt_port;             //
    char  userdata[512];         // User data
} storage_t;

/* *********************************************************************************** */
/* Exported globals                                                                    */
/* *********************************************************************************** */
extern storage_t persistentData;

/* *********************************************************************************** */
/* External Interface                                                                  */
/* *********************************************************************************** */
bool loadFromEEPROM(void);                        // load persistent data from EEPROM
bool saveToEEPROM(void);                          // save persistent data to EEPROM

#endif