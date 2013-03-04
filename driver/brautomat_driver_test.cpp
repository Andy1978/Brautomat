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
  myBrautomat.setvalues.step_temp[0]=50;
  myBrautomat.setvalues.step_time[0]=22;
  myBrautomat.setvalues.step_temp[1]=70;
  myBrautomat.setvalues.step_time[1]=10;
  myBrautomat.setvalues.step_temp[2]=90;
  myBrautomat.setvalues.step_time[2]=15;
  
  myBrautomat.setvalues.temperature_set_point=56.7;
  
	myBrautomat.update();
	myBrautomat.print_setvalues();
	myBrautomat.print_status();
	sleep(1);
  cout << "*******************************" << endl;
	myBrautomat.update();
	myBrautomat.print_setvalues();
	myBrautomat.print_status();

	return 0;
}


