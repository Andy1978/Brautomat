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
  :serial(),
  last_temp_profile_step(0)
{
  //serial.open(device,B57600);
  //serial.open(device,B115200);
  std::cout << "opening serial port " << device << endl;
  serial.open(device,B38400);
  memset(&setvalues,0,sizeof(setvalues));

  update();
}

cBrautomat::~cBrautomat()
{
  std::cout << "closing serial port" << endl;
  serial.close();
  std::cout << "Goodbye" << endl;
}

void cBrautomat::update()
{
  last_temp_profile_step = check_temp_profile();
  if(last_temp_profile_step>-1)
  {
    struct timeval update_ts;
    unsigned int ret=serial.sendData((char*)&setvalues, sizeof(s_setvalues));
    if(DEBUG)
      std::cout << "send "<< ret << " bytes..." << std::endl;
    memset(&status,0,sizeof(s_status));
    gettimeofday(&update_ts, NULL);

    //jetzt mindestens (37+10)/(115200/8) warten (37bytes zum AVR senden, 10Bytes Antwort)
    unsigned int retry_cnt=0;
    const unsigned int max_retry=5;
    unsigned int received_bytes = 0;
    do
      {
        usleep(30000);
        int request_size = sizeof(s_status)-int(received_bytes);
        ret = serial.readData((char*)&status+received_bytes, request_size);
        received_bytes += ret;
        if(DEBUG)
          std::cout << "got "<< ret << " bytes out of requested " << request_size << "..." << std::endl;
      }
    while (received_bytes<sizeof(s_status) && ++retry_cnt<max_retry);
    if(retry_cnt==max_retry)
      {
        std::cerr << "cBrautomat::update(), max retry count überschritten, keine Antwort" << endl;
        exit(-1);
      }
    else
      {
        //status mit timestamp in eine Datei schreiben
        std::ofstream ofs;
        ofs.open ("cBrautomat_status.log", std::ofstream::out | std::ofstream::app);

        ofs << update_ts.tv_sec << " " << update_ts.tv_usec << " "
            << status.temperature
            << " " << status.temperature_set_point
            << " " << int(status.aktive_step)
            << " " << int(status.remaining_step_time)
            << " " << bool(status.bits & 0x01) << std::endl;

        ofs.close();
      }
  }
  else
    std::cerr << "cBrautomat::update(), Temperaturprofil nicht konsistent" << endl;

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
  std::cout << "Letzter Schritt im Temperaturprofil = " << last_temp_profile_step+1 << std::endl;
}

void cBrautomat::print_status()
{
  std::cout << "Isttemperatur     = " << status.temperature << std::endl;
  std::cout << "Solltemperatur    = " << status.temperature_set_point << std::endl;
  std::cout << "aktiver Schritt   = " << int(status.aktive_step) << std::endl;
  std::cout << "verbleibende Zeit = " << int(status.remaining_step_time) << std::endl;
  std::cout << "Heizung aktiv = " << bool(status.bits & 0x01) << std::endl;
}

void cBrautomat::print_steps()
{
  std::cout << "Schritt:" << "  Schrittdauer:\t" << "dT/dt:\t" <<" Solltemperatur:\t" << std::endl;
  for(int i=0; i<MAX_STEPS; i++)
    {
      std::cout << i+1 << "\t  "
        << setvalues.step_time[i] << " s\t\t"
        << setvalues.dT_dt[i] << " °C/min\t"
        << setvalues.step_temp[i] << " °C" << std::endl;
    }
}

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

int cBrautomat::check_temp_profile()
{
  //Plausibilitätsprüfung
  //Wenn dT/dt=0, muss step_time des vorherigen Schritts gleich der des aktuellen Schrittes sein
  //ist step_time=0 und dT/dt=0 ist dies der letzte Schritt der für immer läuft
  for(int n=0; n<MAX_STEPS; ++n)
  {
    if(setvalues.dT_dt[n]==0.0)
    {
      if(n>0 && (setvalues.step_temp[n] != setvalues.step_temp[n-1]))
        return -1;
      if (setvalues.step_time[n]==0.0)
        return n;
    }
  }
  return MAX_STEPS-1;
}

int cBrautomat::set_temp_profile(int step, int step_time, float dT_dt, float step_temp)
{
  if(step>MAX_STEPS)
  {
    cerr << "Schritt " << step << " ueberschreitet die maximale Anzahl von Schritten" << endl;
    return -1;
  }
  if(step<1)
  {
    cerr << "Schritt muss >0 sein" << endl;
    return -1;
  }

  setvalues.step_time[step-1] = step_time;
  setvalues.dT_dt[step-1]     = dT_dt;
  setvalues.step_temp[step-1] = step_temp;
  return 0;
}

void cBrautomat::set_setvalues_bit(uint8_t nr, bool state)
{
  if(state)
    setvalues.bits |= 1<<nr;
  else
    setvalues.bits &= (uint8_t)~(1<<nr);
}
