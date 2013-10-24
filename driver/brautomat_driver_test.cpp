#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include "cBrautomat.h"

using namespace std;

int main()
{
  cout << "Brautomat driver test by Andy" << endl;
  cBrautomat myBrautomat("/dev/ttyUSB0");

  //testweise ein Temperaturprofil vorgeben
  //Wasser mit 20°C/min aufwärmen bis 50°C erreicht
  myBrautomat.setvalues.step_time[0] = 0;
  myBrautomat.setvalues.dT_dt[0]     = 30;
  myBrautomat.setvalues.step_temp[0] = 50;

  //Diese Temperatur für 30s halten
  myBrautomat.setvalues.step_time[1] = 30;
  myBrautomat.setvalues.dT_dt[1]     = 0;
  myBrautomat.setvalues.step_temp[1] = 50;

  //mit 1°C/min auf 60°C aufwärmen
  myBrautomat.setvalues.step_time[2] = 0;
  myBrautomat.setvalues.dT_dt[2]     = 1;
  myBrautomat.setvalues.step_temp[2] = 60;

  //Diese Temperatur für immer halten
  myBrautomat.setvalues.step_time[3] = 0;
  myBrautomat.setvalues.dT_dt[3]     = 0;
  myBrautomat.setvalues.step_temp[3] = 60;

  myBrautomat.setvalues.temperature_set_point=56.7;

  myBrautomat.update();
  myBrautomat.print_setvalues();
  myBrautomat.print_status();
  usleep(1e5);
  cout << "*******************************" << endl;
  myBrautomat.update();
  myBrautomat.print_setvalues();
  myBrautomat.print_status();

  return 0;
}


