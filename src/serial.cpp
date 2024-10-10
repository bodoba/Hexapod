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
#include "serial.h"

void initSerial(void) {

    Serial.begin(115200);

    PRINTLN("");
    PRINTLN("=====================================================================");
    PRINTLN("  ____________                  _              _     _          _");
    PRINTLN("  | ___ \\ ___ \\                | |            | |   | |        | |");
    PRINTLN("  | |_/ / |_/ /   ___ _ __ ___ | |__   ___  __| | __| | ___  __| |");
    PRINTLN("  | ___ \\ ___ \\  / _ \\ '_ ` _ \\| '_ \\ / _ \\/ _` |/ _` |/ _ \\/ _` |");
    PRINTLN("  | |_/ / |_/ / |  __/ | | | | | |_) |  __/ (_| | (_| |  __/ (_| |");
    PRINTLN("  \\____/\\____/   \\___|_| |_| |_|_.__/ \\___|\\__,_|\\__,_|\\___|\\__,_|");
    PRINTLN("");
    PRINTLN("=====================================================================");
    PRINTLN("");
}
