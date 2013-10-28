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
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> //für usleep
#include <math.h>
#include <cstdio>   //für remove
#include <cerrno>   //für errno
#include <sys/time.h>
#include "serial.h"

using namespace libconfig;

typedef unsigned short uint16_t;
typedef short int16_t;
typedef int int32_t;
typedef unsigned char uint8_t;

#define DEBUG 0
#define MAX_STEPS 10

struct s_status
{
  float temperature;            //Isttemperatur [°C]
  float temperature_set_point;  //Solltemperatur [°C]
  uint8_t aktive_step;          //aktueller Schritt im Ablauf
  uint16_t remaining_step_time; //verbleibende Zeit im aktuellen Schritt [s]
  uint8_t	 bits;
  //Bit 0: Heizung aktiv
  uint8_t uart_error;
  uint8_t last_uart_error;      //UART_FRAME_ERROR=8, ART_OVERRUN_ERROR=4, BUFFER_OVERFLOW=2
} __attribute__((__packed__));

struct s_setvalues
{
  float temperature_set_point;   //Solltemperatur [°C]
  int8_t amplitude_set_point;    //Amplitude Rührwerk 0-255
  uint8_t period_set_point;      //Periodendauer Rührwerk in 100ms (0=keine Modulation)
  float step_temp[MAX_STEPS];    //Temperatur in der Schrittkette [°C]
  float dT_dt[MAX_STEPS];        //Temperaturanstieg [°C/min]
  uint16_t step_time[MAX_STEPS]; //Dauer Schritt [s]
  uint16_t add_to_remaining_step_time; //wird zu remaining_step_time
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

  void print_setvalues();
  void print_status();
  void update();

  int load_cfg(const char* filename);
  int save_cfg(const char* filename);

  int set_temp_profile(int step, int step_time, float dT_dt, float step_temp);
  int get_temp_profile(int step, int &step_time, float &dT_dt, float &step_temp);

  int set_temperature(float temp)
  {
    if(temp>99.9)
    {
      cerr << "Sorry, das ist kein Dampfdrucktopf..." << endl;
      return -1;
    }
    setvalues.temperature_set_point = temp;
    update();
    return 0;
  }

  void set_temperature_mode(bool automatic)
  {
    set_setvalues_bit(0, automatic);
    update();
  }

  void set_heater(bool state)
  {
    set_setvalues_bit(1, state);
    update();
  }

  void set_temperatur_from_profile(bool state)
  {
    set_setvalues_bit(2, state);
    update();
  }

  void next_step(bool state)
  {
    set_setvalues_bit(3, state);
    update();
  }

  void previous_step(bool state)
  {
    set_setvalues_bit(4, state);
    update();
  }

  void set_amplitude_set_point(uint8_t amp)
  {
    setvalues.amplitude_set_point = amp;
    update();
  }
  void set_period_set_point(uint8_t period)
  {
    setvalues.period_set_point = period;
    update();
  }

  void add_to_remaining_step_time(uint16_t value)
  {
    //prüfen, ob dies überhaupt ein Schritt mit Weiterschaltung
    //durch Zeit ist
    if(setvalues.step_time[status.aktive_step]>0)
    {
      setvalues.add_to_remaining_step_time = value;
      update();
      setvalues.add_to_remaining_step_time = 0;
      update();
    }
    else
      cerr << "error add_to_remaining_step_time: Schritt hat keine Zeitschaltung" << endl;
  }

  //******************************************************
  float get_temperature()
  {
    return status.temperature;
  }
  float get_temperature_set_point()
  {
    return status.temperature_set_point;
  }
  bool get_heater_state()
  {
    return bool(status.bits & 0x01);
  }

  uint8_t get_aktive_step()
  {
    return status.aktive_step;
  }

  uint16_t get_remaining_step_time()
  {
    return status.remaining_step_time;
  }

private:
  CSerial serial;
  char buffer[4096];

  //AVR relevant
  s_status status;
  s_setvalues setvalues;

  //Allgemein
  string profile_name;

  int check_temp_profile();
  int last_temp_profile_step;

  void print_steps();
  void set_setvalues_bit(uint8_t nr, bool state);
};

#endif
