extern void midi_note(byte chan, byte note, byte vel);
extern void ftdi_write(byte addr, byte *pData, int iLen);
extern int ftdi_read(byte addr, byte *pData, int iLen);

enum
{
  
  EV_GRID_PRESS,
  EV_GRID_RELEASE, 
  EV_RIGHT_PRESS,
  EV_RIGHT_RELEASE, 
  EV_TOP_PRESS,
  EV_TOP_RELEASE, 
  EV_TIMER1,
  EV_TIMER2,
  EV_TIMER3,
  EV_TIMER4,
  EV_TIMER5 
};

class CSketchDriver;

class IPlaypadSketch
{
public:
  virtual void init(CSketchDriver *pDriver)=0;
  virtual void done()=0;
  virtual void handleEvent(int event, int param1, int param2) = 0;  
};

class CSketchDriver 
{
 public:
  IPlaypadSketch *m_pSketch;
  LPDisplay *m_pLP;
  byte midiParam;
  byte m_Channel;
  byte midiMsg[3];
  unsigned short m_Timer1;
  unsigned short m_Timer2;
  unsigned short m_Timer3;
  unsigned short m_Timer4;
  unsigned short m_Timer5;
  
  CSketchDriver() :
    m_Channel(0),
    m_Timer1(0),
    m_Timer2(0),
    m_Timer3(0),
    m_Timer4(0),
    m_Timer5(0) 
    {

      midiMsg[0] = 0x90;
      midiParam = 1;
    }
    
  void run(unsigned long newMillis)
  {
#define SZ_BUFFER 200    
       byte buffer[SZ_BUFFER];
       
       // send any pending updates to launchpad
       int count = m_pLP->_getData(buffer);
       if(count)
         ftdi_write(0, buffer, count);
         
       // read activity from launchpad
       count = ftdi_read(0,buffer,SZ_BUFFER);
       for(int i=0; i<count; ++i)
       {
         if(buffer[i]&0x80)
         {
           midiMsg[0] = buffer[i];
           midiParam = 1;
         }
         else if(midiParam == 1)
         {
           midiMsg[1] = buffer[i];
           midiParam = 2;
         }
         else if(midiParam == 2)
         {
           midiMsg[2] = buffer[i];
           midiParam = 1;
         
           if(midiMsg[0] == (0x90|m_Channel))
           {
             byte a = (midiMsg[1] >> 4)&0x0F;
             byte b = midiMsg[1]&0x0F;
             if(b&0x08)
               m_pSketch->handleEvent(midiMsg[2] ? EV_RIGHT_PRESS : EV_RIGHT_RELEASE, a, 0);
             else
               m_pSketch->handleEvent(midiMsg[2] ? EV_GRID_PRESS : EV_GRID_RELEASE, a, b);
           }
           else if(midiMsg[0] == (0xB0|m_Channel))
           {
             m_pSketch->handleEvent(midiMsg[2] ? EV_TOP_PRESS : EV_TOP_RELEASE, midiMsg[1] - 0x68, 0);
           }
         }
       }
       
       if(newMillis)
       {
         if(m_Timer1 && !(newMillis % m_Timer1))
           m_pSketch->handleEvent(EV_TIMER1,0,0);
         if(m_Timer2 && !(newMillis % m_Timer2))
           m_pSketch->handleEvent(EV_TIMER2,0,0);
         if(m_Timer3 && !(newMillis % m_Timer3))
           m_pSketch->handleEvent(EV_TIMER3,0,0);
         if(m_Timer4 && !(newMillis % m_Timer4))
           m_pSketch->handleEvent(EV_TIMER4,0,0);
         if(m_Timer5 && !(newMillis % m_Timer5))
           m_pSketch->handleEvent(EV_TIMER5,0,0);
       }
  }
};


