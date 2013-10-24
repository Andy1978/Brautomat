#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include "../driver/cBrautomat.h"

using namespace std;

int main()
{
  cout << "# PT100 calibration by Andy" << endl;
  cout << "# put PT100 and DS18B20 into boiling water" << endl;
  cBrautomat myBrautomat("/dev/ttyUSB0");

  cout << "# unix timestamp, status.temperature, status.rawPT100" << endl;
  while(1)
  {
    time_t t=time(NULL);
    myBrautomat.update();
    cout << t << " " << myBrautomat.status.temperature << " " << myBrautomat.status.rawPT100/64.0 << endl;
    //usleep(500000);
    sleep(1);
  }
  return 0;
}


