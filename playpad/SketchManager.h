
////////////////////////////////////////////////////////////////////////////
//
// SKETCH MANAGER
//
// MANAGES READING THE USB HOST AND PASSING EVENTS TO 
// THE SKETCHES
//
////////////////////////////////////////////////////////////////////////////

#define SPI_SSPIN 10 //TODO
class CSketchManager
{
  
  VNC2SPI m_vncspi;
public:
  CSketchManager()
  {
    m_vncspi.begin(SPI_SSPIN)
  }
  void run(unsigned long millis)
  {
    
    read(byte addr, byte *pData, int iLen)
  }
  
};
