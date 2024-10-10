/* *********************************************************************************** */
/*                                                                                     */
/*  serial communication shortcuts                                                     */
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

#ifndef SERIAL_H
#define SERIAL_H

#define PRINT(a)      Serial.print(a)
#define PRINTHEX(a)   Serial.print(a, HEX)
#define PRINTLN(a)    Serial.println(a)
#define PRINTHEXLN(a) Serial.println(a, HEX)

void initSerial(void);

#endif
