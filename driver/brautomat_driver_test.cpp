#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include "cBrautomat.h"

using namespace std;

int main()
{
  cout << "Brautomat driver test by Andy" << endl;
  cBrautomat *myBrautomat = new cBrautomat("/dev/ttyUSB0");

  const char fn[]="temp_profile.cfg";

  //testweise ein Temperaturprofil vorgeben
  //Wasser mit 20°C/min aufwärmen bis 40°C erreicht
  //~ myBrautomat->set_temp_profile(1, 0, 30, 40);
//~
  //~ //Diese Temperatur für 30s halten
  //~ myBrautomat->set_temp_profile(2, 30, 0, 40);
//~
  //~ //mit 1°C/min auf 60°C aufwärmen
  //~ myBrautomat->set_temp_profile(3, 0, 1, 45);
//~
  //~ //Diese Temperatur für 25s halten
  //~ myBrautomat->set_temp_profile(4, 20, 0, 45);
//~
  //~ //mit 2°C/min auf 65°C aufwärmen
  //~ myBrautomat->set_temp_profile(5, 0, 2, 50);
//~
  //~ //Diese Temperatur für immer halten
  //~ myBrautomat->set_temp_profile(6, 0, 0, 50);

  myBrautomat->set_temp_profile(1, 0,  10, 50);
  myBrautomat->set_temp_profile(2, 120, 0, 50);
  myBrautomat->set_temp_profile(3, 0,   3, 62);
  myBrautomat->set_temp_profile(4, 180, 0, 62);
  myBrautomat->set_temp_profile(5,  0,  3, 72);
  myBrautomat->set_temp_profile(6, 180, 0, 72);
  myBrautomat->set_temp_profile(7,  0,  3, 78);
  myBrautomat->set_temp_profile(8,  0,  0, 78);

  myBrautomat->save_cfg(fn);
  delete(myBrautomat);

  //*********************************************
  cBrautomat *myBrautomat2 = new cBrautomat("/dev/ttyUSB0");
  myBrautomat2->load_cfg(fn);

  myBrautomat2->update();
  //myBrautomat->print_setvalues();
  //myBrautomat->print_status();
  usleep(1e5);
  myBrautomat2->update();
  myBrautomat2->print_setvalues();
  cout << "*********************************************************" << endl;
  myBrautomat2->print_status();

  //myBrautomat2->set_temperature(10);
  //myBrautomat2->set_heater(1);
  //sleep(1);
  //myBrautomat2->set_heater(0);
  sleep(1);
  myBrautomat2->set_temperatur_from_profile(1);
  myBrautomat2->set_temperature_mode(1);

  sleep(350);
  myBrautomat2->set_temperatur_from_profile(0);
  myBrautomat2->set_temperature_mode(0);
  delete(myBrautomat2);
  return 0;
}


