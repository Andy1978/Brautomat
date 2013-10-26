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
  myBrautomat.set_temp_profile(1, 0, 30, 50);

  //Diese Temperatur für 30s halten
  myBrautomat.set_temp_profile(2, 30, 0, 50);

  //mit 1°C/min auf 60°C aufwärmen
  myBrautomat.set_temp_profile(3, 0, 1, 60);

  //Diese Temperatur für 25s halten
  myBrautomat.set_temp_profile(4, 20, 0, 60);

  //mit 2°C/min auf 65°C aufwärmen
  myBrautomat.set_temp_profile(5, 0, 2, 65);

  //Diese Temperatur für immer halten
  myBrautomat.set_temp_profile(6, 0, 0, 65);

  myBrautomat.update();
  //myBrautomat.print_setvalues();
  //myBrautomat.print_status();
  usleep(1e5);
  myBrautomat.update();
  myBrautomat.print_setvalues();
  cout << "*********************************************************" << endl;
  myBrautomat.print_status();

  myBrautomat.set_temperature(60);
  myBrautomat.set_heater(1);
  sleep(1);
  myBrautomat.set_heater(0);
  sleep(1);
  myBrautomat.set_temperatur_from_profile(1);
  sleep(1);
  myBrautomat.set_temperatur_from_profile(0);

  return 0;
}


