// Serial.h
#include <termios.h>

#ifndef __SERIAL_H__
#define __SERIAL_H__

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

//debugging
#include <iostream>
using namespace std;

class CSerial
{

public:
  CSerial();
  ~CSerial();

  //! mit der Baudrate speed verbinden. ACHTUNG: "B" voranstellen da Makro, z.B. B19200
  int open(const char* device, int speed= B9600);
  int close( void );

  int readData( void *, int);
  int readDataBlocking( void *, int );
  int sendData( const char*, int );

  int isOpened( void )
  {
    return( m_fd >= 0 ? 1 : 0 );
  }

protected:
  int m_fd;
};

#endif
