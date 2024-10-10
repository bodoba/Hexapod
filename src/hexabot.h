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

#ifndef HEXABOT_H
#define HEXABOT_H

// Modes used to interpret incoming commands
#define MODE_WALK    'A'       // Tripod Gait
#define MODE_RIPPLE  'B'       // Ripple Gait
#define MODE_QUAD    'C'       // Quad Gait
#define MODE_WAVE    'D'       // Waving motions

#define SUBMODE_1    '1'
#define SUBMODE_2    '2'
#define SUBMODE_3    '3'
#define SUBMODE_4    '4'

// commands to move the robot
#define COMMAND_FORWARD  'f'
#define COMMAND_BACKWARD 'b'
#define COMMAND_RIGHT    'r'
#define COMMAND_LEFT     'l'
#define COMMAND_STAND    's'
#define COMMAND_STOMP    'w'
#define COMMAND_NONE     ' '

#endif