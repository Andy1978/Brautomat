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

#include "cBrautomat.h"

cBrautomat::cBrautomat(const char* device)
  :serial()
{
  //serial.open(device,B57600);
  //serial.open(device,B115200);
  serial.open(device,B38400);
  memset(&setvalues,0,sizeof(setvalues));
  //_set_servos(0,0);
}

void cBrautomat::update()
{
  unsigned int ret=serial.sendData((char*)&setvalues, sizeof(s_setvalues));
  if(DEBUG)
    std::cout << "send "<< ret << " bytes..." << std::endl;
  memset(&status,0,sizeof(s_status));

  //jetzt mindestens (37+10)/(115200/8) warten (37bytes zum AVR senden, 10Bytes Antwort)
  unsigned int retry_cnt=0;
  const unsigned int max_retry=5;
  do
    {
      usleep(30000);
      ret = serial.readData((char*)&status, sizeof(s_status));
      if(DEBUG)
        std::cout << "got "<< ret << " bytes..." << std::endl;
    }
  while (ret<sizeof(s_status) && ++retry_cnt<max_retry);
  if(retry_cnt==max_retry)
    {
      std::cerr << "cBrautomat::update(), max retry count überschritten, keine Antwort" << endl;
      exit(-1);
    }

}

void cBrautomat::print_setvalues()
{
  std::cout << "Solltemperatur           = " << setvalues.temperature_set_point << std::endl;
  std::cout << "Sollamplitude            = " << int(setvalues.amplitude_set_point) << std::endl;
  std::cout << "Sollperiodendauer        = " << int(setvalues.period_set_point) << std::endl;
  std::cout << "Temperaturregelung aktiv             = " << bool(setvalues.bits & 0x01) << std::endl;
  std::cout << "Heizung aktiv im Handbetrieb         = " << bool(setvalues.bits & 0x02) << std::endl;
  std::cout << "Temperatursollwerte aus Schrittkette = " << bool(setvalues.bits & 0x04) << std::endl;
  print_steps();
}

void cBrautomat::print_status()
{
  std::cout << "Isttemperatur     = " << status.temperature << std::endl;
  std::cout << "aktiver Schritt   = " << int(status.aktive_step) << std::endl;
  std::cout << "verbleibende Zeit = " << int(status.remaining_step_time) << std::endl;
  std::cout << "Heizung aktiv = " << bool(status.bits & 0x01) << std::endl;
}

void cBrautomat::print_steps()
{
  for(int i=0; i<5; i++)
    {
      std::cout << "Schritt:" << i << " Solltemp. = " << setvalues.step_temp[i];
      std::cout << ", Schrittdauer = " << setvalues.step_time[i] << "s" << std::endl;
    }
}

/*
void cBrautomat::_set_servos(float pos1, float pos2)
{
    pos1=clamp_abs(-pos1,95)/90;
    pos2=clamp_abs(-pos2,95)/90;

    setvalues.servo2_pos_soll=(pos2>=0)? pos2*(990-570)+570 : pos2*(570-130)+570;
    setvalues.servo1_pos_soll=(pos1>=0)? pos1*(75-538)+538 : pos1*(538-955)+538;

    setvalues.enable |= 0x03;		//PWM Freigabe für beide Servos
    update();
    setvalues.enable &= 0xFC;
	update();
}

void cBrautomat::_set_servos_wait(float pos1, float pos2)
{
    _set_servos(pos1,pos2);

	//Status flag pollen. AVR setzt diese zurück sobald Position erreicht
	while(status.enable & 3)
	{
	 update();
	 usleep(10000);
     //cout << "waiting--" << hex << uint16_t(status.enable) <<  endl;
	}
}

void cBrautomat::set_servos(float pos1, float pos2)
{
    _set_servos_wait(pos1+1.8,pos2+1.8);
    usleep(300000);
    _set_servos_wait(pos1,pos2);
    update();
}

void cBrautomat::set_QWP_rotation(double rot)
{
	update();
	double diff=rot-get_QWP_rotation();
	if(diff<0)
	{
		if(DEBUG)
			cout << "diff < 0, diff=" << diff << endl;
		rot+=(-trunc(diff/360)+1)*360;
		if(DEBUG)
			cout << "using " << rot << " as new value" << endl;
	}

	setvalues.qwp_pos=_qwp_deg_to_cnt(rot);
	setvalues.enable |= 1<<2;
	update();
	setvalues.enable &= ~(1<<2);
}

*/

int cBrautomat::load_cfg(const char* filename)
{
  Config cfg;
  //try to read the file
  try
    {
      cfg.readFile(filename);
      cfg.lookupValue("profile_name", profile_name);

      Setting &step_temp = cfg.lookup("temp_profile.step_temp");
      Setting &dT_dt  = cfg.lookup("temp_profile.dT_dt");
      Setting &step_time  = cfg.lookup("temp_profile.step_time");
      if(    (step_temp.getLength() != dT_dt.getLength())
             || (dT_dt.getLength()  != step_time.getLength())
             || (step_time.getLength()  != step_temp.getLength()))
        {
          cerr << "error: length of step_temp, dT_dt and step_time has to be equal\\n" << endl;;
          return -1;
        }
      if(step_temp.getLength() != MAX_STEPS)
        {
          cerr << "error: length of step_temp, dT_dt and step_time has to be \\n" << MAX_STEPS << endl;;
          return -1;
        }
      for (int i=0; i<MAX_STEPS; ++i)
        {
          setvalues.step_temp[i] = float(step_temp[i]);
          setvalues.dT_dt[i]     = float(dT_dt[i]);
          setvalues.step_time[i] = float(step_time[i]);
        }
    }
  catch (FileIOException &fioex) //file does not exist or parse error
    {
      std::cerr << "File I/O error " << filename << std::endl;
      return(EXIT_FAILURE);
    }
  catch(const ParseException &pex)
    {
      cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
           << " - " << pex.getError() << std::endl;
      return(EXIT_FAILURE);
    }
  return(EXIT_SUCCESS);
}

int cBrautomat::save_cfg(const char* filename)
{
  //try to remove the file
  int ret = std::remove (filename);
  if(!ret)
    cout << "INFO: removed file " << filename << endl;
  else
    cout << "INFO: couldn't remove " << filename << ". MSG: " << strerror(errno) << endl;

  try
    {
      Config cfg;
      Setting &root = cfg.getRoot();
      root.add("profile_name", Setting::TypeString) = profile_name;
      Setting &temp_profile = root.add("temp_profile", Setting::TypeGroup);
      Setting &step_temp    = temp_profile.add("step_temp", Setting::TypeArray);
      Setting &dT_dt        = temp_profile.add("dT_dt", Setting::TypeArray);
      Setting &step_time    = temp_profile.add("step_time", Setting::TypeArray);

      for (unsigned int i=0; i<MAX_STEPS; ++i)
        {
          step_temp.add(Setting::TypeFloat) = setvalues.step_temp[i];
          dT_dt.add(Setting::TypeFloat)     = setvalues.dT_dt[i];
          step_time.add(Setting::TypeFloat) = setvalues.step_time[i];
        }
      cfg.writeFile(filename);
    }
  catch(const FileIOException &fioex)
    {
      cerr << "I/O error while writing file: " << filename << endl;
      return(EXIT_FAILURE);
    }
  return(EXIT_SUCCESS);
}
