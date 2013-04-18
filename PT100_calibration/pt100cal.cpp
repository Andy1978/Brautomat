#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include "../driver/cBrautomat.h"

using namespace std;

int main()
{
  cout << "# PT100 calibration by Andy" << endl;
  cout << "# put PT100 and DS18B20 into boiling water" << endl;
  cBrautomat myBrautomat("/dev/ttyUSB0");

  cout << "# status.temperature status.rawPT100" << endl;
  while(1)
  {
    myBrautomat.update();
    cout << myBrautomat.status.temperature << " " << myBrautomat.status.rawPT100/100.0 << endl;
    //sleep(1);
  }
  return 0;
}


