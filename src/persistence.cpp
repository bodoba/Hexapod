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

#include "persistence.h"

/* *********************************************************************************** */
/* Global variables                                                                    */
/* *********************************************************************************** */
storage_t persistentData;

/* *********************************************************************************** */
/*  @brief Load persistent data from EEPROM                                            */
/* *********************************************************************************** */
bool loadFromEEPROM(void) {
    char *dest = (char *)&persistentData;
    
    // copy data from EEPROM into data struct;
    for (int address = 0; address<EEPROM_SIZE; address++) {
        *dest = EEPROM.read(address);
        address++;
        dest++;
    }

    // check if data is valid
    return !strncmp(EEPROM_MAGIC, persistentData.magic, strlen(EEPROM_MAGIC));
}

/* *********************************************************************************** */
/*  @brief Save persistent data to EEPROM                                              */
/* *********************************************************************************** */
bool saveToEEPROM(void) {
    char *src = (char *)&persistentData;
    
    // copy data from EEPROM into data struct;
    for (int address = 0; address<EEPROM_SIZE; address++) {
        EEPROM.write(address, *src);
        address++;
        src++;
    }
    return EEPROM.commit();
}
