/*
    Copyright 2013 Andreas Weber (andy.weber.aw at gmail dot com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
  Brautomat driver class PC<->AVR communication

  Temperaturprofil: Es gibt 3 Werte:

  step_time:
    Nach Ablauf dieser Zeit wird der nächste Schritt aktiviert.
    Ist step_time = 0, so wird durch step_temp(siehe dort) weiter geschaltet.
  dT/dt:
    Anstieg der Solltemperatur
  step_temp
    Bei Überschreiten (dT/dt>0) oder Unterschreiten (dT/dt<0)
    dieser Temperatur wird der nächste Schritt aktiviert.
    Ist dT/dt=0 so wird nur anhand step_time weiter geschaltet

  Ein typisches Profil wäre z.B.
  step_time dT/dt step_temp Erklärung
  0         30    50        schnelles Aufwärmen des Wassers bis 50°C
  600       0     50        Einmaischen + 1.Rast
  0         1     62        Aufheizen 2
  1800      0     62        2. Rast
  0         1     72        Aufheizen 3
  1800      0     72        3. Rast
  0         1     78        Aufheizen auf Läutertemperatur
  0         0     78        läuft für immer

  Plausibilitätsprüfung
  * Wenn dT/dt=0, muss step_time des vorherigen Schritts gleich der des aktuellen Schrittes sein
*/

#ifndef _C_BRAUTOMAT_H
#define _C_BRAUTOMAT_H

#include <libconfig.h++>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> //für usleep
#include <math.h>
#include <cstdio>   //für remove
#include <cerrno>   //für errno
#include "serial.h"

using namespace libconfig;

typedef unsigned short uint16_t;
typedef short int16_t;
typedef int int32_t;
typedef unsigned char uint8_t;

#define DEBUG 1
#define MAX_STEPS 10

struct s_status
{
  float temperature;            //Isttemperatur [°C]
  uint8_t aktive_step;          //aktueller Schritt im Ablauf
  uint16_t remaining_step_time; //verbleibende Zeit im aktuellen Schritt [s]
  uint8_t	 bits;
  //Bit 0: Heizung aktiv
  uint8_t uart_error;
  uint8_t last_uart_error;      //UART_FRAME_ERROR=8, ART_OVERRUN_ERROR=4, BUFFER_OVERFLOW=2
} __attribute__((__packed__));

struct s_setvalues
{
  float temperature_set_point;  //Solltemperatur [°C]
  uint8_t amplitude_set_point;  //Amplitude Rührwerk 0-255
  uint8_t period_set_point;     //Periodendauer Rührwerk in 100ms (0=keine Modulation)
  float step_temp[MAX_STEPS];   //Temperatur in der Schrittkette [°C]
  float dT_dt[MAX_STEPS];       //Temperaturanstieg [°C/min]
  uint16_t step_time[MAX_STEPS]; //Dauer Schritt [s]
  uint8_t	 bits;
  //Bit 0: Temperaturregelung aktiv (Handbetrieb wenn nicht)
  //Bit 1: Heizung aktiv im Handbetrieb
  //Bit 2: Temperatursollwerte aus Schrittkette
  //Bit 3: Schritt weiter (Flanke)
  //Bit 4: Schritt zurück (Flanke)
} __attribute__((__packed__));

class cBrautomat
{
public:
  cBrautomat(const char* device);
  ~cBrautomat();

  //AVR relevant
  s_status status;
  s_setvalues setvalues;

  //Allgemein
  string profile_name;

  void print_setvalues();
  void print_status();
  void print_steps();
  void update();

  int load_cfg(const char* filename);
  int save_cfg(const char* filename);

private:
  CSerial serial;
  char buffer[4096];
};

#endif
