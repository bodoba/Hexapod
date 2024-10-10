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
#include "wifi.h"
#include "mqtt.h"
#include "serial.h"

/* *********************************************************************************** */
/* Global variables                                                                    */
/* *********************************************************************************** */
static char inTopic[256];
static char buffer[512];

extern service_t mqttServiceList[]; // list of callback functions for incoming MQTT messages.

/* *********************************************************************************** */
/* Local Prototypes                                                                    */
/* *********************************************************************************** */

void mqttCallback ( char* topic, byte* payload, unsigned int length);

/* *********************************************************************************** *
 * @brief Connect to MQTT broker                                                       
 *                                                                                     
 * Make 10 attempts to connect to the MQTT broker                                      
 *                                                                                     
 * @retval 'true'   on success,                                                         
 *         'false'  if no connection could be established                              
 * *********************************************************************************** */
bool initMQTT(void) {
    uint8_t count = 1;
    bool connected = client.connected();

    // Loop 10 times until we're reconnected
    while ( !connected && count<=10 ) {
        // PRINT("Attempting MQTT connection...");
        // Attempt to connect
        sprintf(inTopic, "/%s/Command/#", myId);

        client.setServer(MQTT_BROKER, MQTT_PORT);

        if (client.connect(myId)) {
            PRINT( "Connected to MQTT broker " );
            PRINT( MQTT_BROKER ); PRINT(":"); PRINT(MQTT_PORT);
            client.subscribe(inTopic);
            PRINT( " and subscribed to topic: " );
            PRINTLN( inTopic );
            connected = true;
            client.setCallback(mqttCallback);
        } else {
            // Wait 5 seconds before retrying
            delay(5000);
            count++;
        }
    }

    return connected;
}

/* *********************************************************************************** *
 * @brief Send debug message over MQTT                                                       
 * 
 * A message with the topic /<myId>/Debug will be sent to the broker
 *                                                                                     
 * @param  message  The debug message. A %s in the message will be replaced by 'myId'
 * *                                                                                    
 * @retval 'true'   if message has been sent,                                                         
 *         'false'  if sending the MQTT message failed                              
 * *********************************************************************************** */
bool mqttDebug( const char* message ) {
    bool retval = true;

    if ( initMQTT() ) {
        char outTopic[64];
        char outMessage[128];
        
        sprintf (outTopic,   "/%s/Debug", myId);
        sprintf (outMessage, "%s", message);
        
        if (!client.publish(outTopic, outMessage)) {
            retval = false;
        }
        client.loop();
    }
    return retval;
}

/* *********************************************************************************** *
 * @brief Send debug message over MQTT                                                       
 * 
 * A the given message with the given topic will be sent to the broker
 *                                                                                     
 * @param  topicFmt The topic to be used. A %s will be replaced by 'myId'
 * @param  message  The intended message
 *                                                
 * @retval 'true'   if message has been sent,                                                         
 *         'false'  if sending the MQTT message failed                              
 * *********************************************************************************** */
bool mqttSendMessage( const char* topicFmt, const char* message ) {
    bool retval = true;

    if ( initMQTT() ) {
        char outTopic[64];
        
        sprintf (outTopic, topicFmt, myId);
        
        if (!client.publish(outTopic, message)) {
            retval = false;
        }
        client.loop();
    }
    return retval;
}

/* *********************************************************************************** *
 * @brief Handle incoming MQTT messages
 *
 * Generic MQTT callback for all messages matching the topic "/<myId>/Command/#
 * If <mqttServiceList> contains a matching topic, the refenced function will be called.
 * *********************************************************************************** */
void mqttCallback ( char* topic, byte* payload, unsigned int length) {
    payload[length] = (char)0;
    memcpy( (char*)buffer, (char*)payload, length);
    buffer[length] = (char)0;
    boolean match = false;
    
    PRINTLN("MQTT Callback");

    if ( strlen(topic) > strlen(inTopic)-1) {
        char *command=topic+strlen(inTopic)-1;

        PRINT("Received MQTT command: <"); PRINT(command); PRINTLN(">");
        
        for ( int index = 0; mqttServiceList[index].name != NULL; index++ ) {

            PRINT("Checking for servive <");PRINT(mqttServiceList[index].name);PRINTLN(">");

            if ( !strncmp(mqttServiceList[index].name, command, strlen(mqttServiceList[index].name))) {
                match = true;

                PRINTLN("Found match!");

                if (mqttServiceList[index].handler) {
                    (mqttServiceList[index].handler)(buffer);
                }
            }
        }
        
        if ( !match ) {
            PRINTLN("ERROR: Received unknown MQTT command" );
        }
    }
}