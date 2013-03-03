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
    uint16_t temperature; //ToDo: skalierung einfügen
    uint8_t	 enable;
} __attribute__((__packed__));

struct s_setvalues
{
    uint16_t temperature_set_point;
    uint16_t amplitude_set_point;
    uint16_t period_set_point;
    uint8_t	 enable;
} __attribute__((__packed__));

class cBrautomat
{
public:
    cBrautomat(const char* device);

    s_status status;
    s_setvalues setvalues;

    void temperature_set_point(double t);
    double temperature_set_point();
    
    void print_setvalues();
    void print_status();
    void update();

private:
    CSerial serial;
    char buffer[4096];
};

#endif
