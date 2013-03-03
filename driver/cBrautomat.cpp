#include "cBrautomat.h"


cBrautomat::cBrautomat(const char* device)
    :serial()
{
  serial.open(device,B57600);
  memset(&setvalues,0,sizeof(setvalues));
  //_set_servos(0,0);
}

void cBrautomat::temperature_set_point(double t)
{
  //TODO: ggf. auf 100°C begrenzen?

  //TODO: Skalierung...
  setvalues.temperature_set_point= 4321;

  //setvalues.enable |= 0x03;		//PWM Freigabe für beide Servos
  //update();
  //setvalues.enable &= 0xFC;
  //update();
}

void cBrautomat::update()
{
  int ret=serial.sendData((char*)&setvalues, sizeof(s_setvalues));
  if(DEBUG)
    std::cout << "send "<< ret << " bytes..." << std::endl;
  memset(&status,0,sizeof(s_status));
  ret=serial.readDataBlocking((char*)&status, sizeof(s_status));
  if(DEBUG)
    std::cout << "got "<< ret << " bytes..." << std::endl;
}

void cBrautomat::print_setvalues()
{
    std::cout << "Solltemperatur           = " << setvalues.temperature_set_point << std::endl;
    std::cout << "Sollamplitude            = " << setvalues.amplitude_set_point << std::endl;
    std::cout << "Sollperiodendauer        = " << setvalues.period_set_point << std::endl;
    std::cout << "Temperaturregelung aktiv = " << bool(setvalues.enable & 0x01) << std::endl;
}

void cBrautomat::print_status()
{
    std::cout << "Isttemperatur = " << status.temperature << std::endl;
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
