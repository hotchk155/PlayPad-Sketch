#ifndef _VNC2SPI_H_
#define _VNC2SPI_H_

class VNC2SPI
{
  enum {
    VNC2_WRITE  = 0x00,
    VNC2_READ   = 0x10,
    VNC2_TXE    = 0x08,
    VNC2_RXF    = 0x04,
    VNC2_ACK    = 0x02
  };
  byte m_ssPin;
  byte m_addr;
public:  
  void begin(byte ssPin, byte addr = 0);
  void write(byte addr, byte *pData, int iLen);
  int read(byte addr, byte *pData, int iLen);
};

#endif // _VNC2SPI_H_
