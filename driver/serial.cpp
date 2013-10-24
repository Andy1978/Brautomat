#include "serial.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>


CSerial::CSerial()
 :m_fd(-1) {}

CSerial::~CSerial()
{
  if(m_fd >= 0)
    close();
}

int CSerial::open(const char* device, int speed)
{
  struct termios tio;

  if(m_fd >=0)
    return -2;

  m_fd = ::open(device, O_RDWR | O_NOCTTY );
  if (m_fd <0) {
        perror(device);
        return m_fd;
    }
    bzero(&tio, sizeof(tio));

  tio.c_cflag = speed | CS8 | CLOCAL | CREAD;
    tio.c_iflag = IGNPAR;
    tio.c_oflag = 0;
  // set input mode (non-canonical, no echo,...)
    tio.c_lflag = 0;

  tio.c_cc[VTIME]    = 0;   // inter-character timer unused
    tio.c_cc[VMIN]     = 0;  // blocking read until 5 chars received

  tcflush(m_fd, TCIFLUSH);
    tcsetattr(m_fd,TCSANOW,&tio);

  //set the RTS Pin
  int status;
    ioctl(m_fd, TIOCMGET, &status);
    status |= TIOCM_RTS;
  ioctl(m_fd, TIOCMSET, &status);

  return m_fd;
}

int CSerial::close( void )
{
  if(m_fd >= 0)
  {
    int tmp= m_fd;
    m_fd= -1;
    return ::close(tmp);
  }
  return -1;
}

int CSerial::readData( void * data, int size)
{
  if(m_fd < 0)
    return -1;
  return read(m_fd, data, size);
}

int CSerial::readDataBlocking( void * data, int size)
{
  if(m_fd < 0)
    return -1;
  //Debugging, auf Nummer sicher gehen...
  memset(data,0,size);

  int len;
  int read_count=0;
  char* d=(char*)data;
  do
  {
    while(!(len=read(m_fd, d+read_count, size-read_count)));
    read_count+=len;
  }while(read_count<size);
  return read_count;
}

int CSerial::sendData( const char* data, int size)
{
  if(m_fd < 0)
    return -1;
  return write(m_fd, data, size);
}


