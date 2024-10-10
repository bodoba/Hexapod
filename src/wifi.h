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
#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>  
#include <ArduinoOTA.h>

#include "persistence.h"

#ifndef WIFI_H
#define WIFI_H

/* *********************************************************************************** */
/* Network Settings                                                                    */
/* *********************************************************************************** */
#define WIFISSID    "BB embedded - SETUP"
#define MQTT_BROKER "192.168.100.26"
#define MQTT_PORT   1883

/* *********************************************************************************** */
/* Exported globals                                                                    */
/* *********************************************************************************** */
extern WiFiClient   wifiClient;
extern WiFiManager  wifiManager;
extern PubSubClient client;
extern bool         otaActive;
extern char         myId[10];

/* *********************************************************************************** */
/* Prototypes                                                                          */
/* *********************************************************************************** */
void initWifi(void);                           // Board Initialisation, Wifi Connect
void resetWifi(void);                          // Force WLAN to be reset
void initOTA(void);                            // Allow update over the air

#endif