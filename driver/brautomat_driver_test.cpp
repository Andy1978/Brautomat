#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include "cBrautomat.h"

using namespace std;

int main()
{
  cout << "Brautomat driver test by Andy" << endl;
	cBrautomat myBrautomat("/dev/ttyUSB1");
  
	myBrautomat.update();
	myBrautomat.print_setvalues();
	myBrautomat.print_status();
	sleep(1);
	myBrautomat.update();
	myBrautomat.print_setvalues();
	myBrautomat.print_status();

	return 0;
}


