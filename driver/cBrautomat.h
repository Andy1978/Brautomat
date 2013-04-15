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
*/

#ifndef _C_BRAUTOMAT_H
#define _C_BRAUTOMAT_H

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> //für usleep
#include <math.h>
#include "serial.h"

typedef unsigned short uint16_t;
typedef short int16_t;
typedef int int32_t;
typedef unsigned char uint8_t;

#define DEBUG 0

struct s_status
{
  float temperature;            //Isttemperatur [°C]
  int16_t rawPT100;             //Rohwert PT100, differential
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
  float step_temp[5];           //Temperatur in der Schrittkette
  uint16_t step_time[5];        //Dauer Schritt
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

    s_status status;
    s_setvalues setvalues;

    void print_setvalues();
    void print_status();
    void print_steps();
    void update();

private:
    CSerial serial;
    char buffer[4096];
};

#endif
