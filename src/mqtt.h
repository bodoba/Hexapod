/* *********************************************************************************** */
/*                                                                                     */
/*  Convenience funtions for MQTT communication                                        */
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

#ifndef MQTT_H
#define MQTT_H

#define MQTT_BROKER "192.168.100.26"
#define MQTT_PORT   1883

/* *********************************************************************************** *
 * Custom Types                                                                        *
 * *********************************************************************************** */
typedef struct {     // struct specifing a callback function fot a specific MQTT topic
    const char* name;
    void (*handler)(const char*);
} service_t;

/* ********************************************************************************** * 
 * Prototypes                                                                         * 
 * ********************************************************************************** */
bool initMQTT(void);                                    // initialize mqtt communication

bool mqttDebug( const char* message );                   // send Debug message over MQTT
bool mqttSendMessage( const char* topicFmt, const char* message );  // send MQTT message 

#endif