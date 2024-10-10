/* *********************************************************************************** */
/*                                                                                     */
/*  Initialize and handle WiFi on ESP8266 board                                        */
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
#include "serial.h"

/* *********************************************************************************** */
/* Global variables                                                                    */
/* *********************************************************************************** */
WiFiClient   wifiClient;
WiFiManager  wifiManager;

PubSubClient client(wifiClient);
char         myId[10];
bool         otaActive = false;

/* *********************************************************************************** */
/*  @brief connect to Wifi network                                                     */
/* *********************************************************************************** */
void initWifi(void) {
    byte mac_address[6];
    char port[7];

    sprintf(port, "%d", persistentData.mqtt_port);
    WiFiManagerParameter paramMqttBroker("mqtt_broker", "MQTT Broker", persistentData.mqtt_broker, 40, "Broker");
    WiFiManagerParameter paramMqttPort("mqtt_port", "MQTT Port", port, 6, "Port");

    wifiManager.setConfigPortalTimeout(600);
    wifiManager.addParameter(&paramMqttBroker);
    wifiManager.addParameter(&paramMqttPort);

    while (!wifiManager.autoConnect(WIFISSID)) {  
        // failed to connect, reset and try again...
        delay(3000);
        ESP.reset();
        delay(5000);
    }

    strcpy(persistentData.mqtt_broker, paramMqttBroker.getValue());
    persistentData.mqtt_port = atoi(paramMqttPort.getValue());    
    strcpy(persistentData.magic, EEPROM_MAGIC);
    WiFi.macAddress(mac_address);
    sprintf(myId, "BB-%02x%02x", mac_address[4], mac_address[5]);

    saveToEEPROM();
}

/* *********************************************************************************** */
/*  @brief Force WLAN to be reconfigured                                                      */
/* *********************************************************************************** */
void resetWifi(void) {
    wifiManager.resetSettings();
    delay(500);
    ESP.reset();
}

/* *********************************************************************************** */
/* @brief Allow update over the air                                                    */
/* *********************************************************************************** */
void initOTA(void) {
    // Port defaults to 8266
    ArduinoOTA.setPort(8266);

    // Set Hostname
    ArduinoOTA.setHostname(myId);

    ArduinoOTA.onStart([](void) {
        PRINTLN("Start OTA Update");
    });
    
    ArduinoOTA.onEnd([](void) {
        digitalWrite(LED_BUILTIN, HIGH);
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        char message[] = "Progress: xxxxxxxxxxx";
        sprintf(message, "Progress: %u%%\r", (progress / (total / 100)));
        PRINT (message);
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
        PRINT("Error["); PRINT(error); PRINT("]: ");
        if (error == OTA_AUTH_ERROR)         PRINTLN("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)   PRINTLN("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) PRINTLN("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) PRINTLN("Receive Failed");
        else if (error == OTA_END_ERROR)     PRINTLN("End Failed");
    });
    
    ArduinoOTA.begin();
    otaActive = true;
}
