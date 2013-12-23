/////////////////////////////////////////////////////
//
// Based class and interface for "sketches" that run
// on the hib
//
/////////////////////////////////////////////////////
class CPlaypadSketch
{  
protected:  
  enum { MAX_TIMERS = 4 };
  unsigned long m_nextTimerEvent[MAX_TIMERS];
  unsigned int m_timerPeriod[MAX_TIMERS];
  byte m_midiChannel;
  COutputDriver *m_pOutput;

public:  
  enum { 
      INPUT_CH_LPA = 0,   // input comes from Launchpad on PORTA
      INPUT_CH_LPB = 1,   // input comes from Launchpad on PORTB
      INPUT_CH_MIDI = 2    // input comes from the MIDI port
  };
  enum {
    EV_INIT = 1,
    EV_TICK,
    EV_RESTART,
    EV_ARROWUP,
    EV_ARROWDOWN,
    EV_ARROWLEFT,
    EV_ARROWRIGHT,
    EV_TIMER0 = 100
  };

  CPlaypadSketch()
  {
    memset(m_nextTimerEvent, 0, sizeof(m_nextTimerEvent));
    memset(m_timerPeriod, 0, sizeof(m_timerPeriod));
  }
  void setOutput(COutputDriver *pOutput)
  {
    m_pOutput = pOutput;
  }
  void setMidiChannel(byte midiChannel)
  {
    m_midiChannel = midiChannel;
  }
       
  // trigger timer events
  void handleTimer(unsigned long ms)
  {
    for(int i=0;i<MAX_TIMERS;++i)
    {
      if(m_timerPeriod[i])
      {
        if(m_nextTimerEvent[i] < ms)
        {
          handleEvent(EV_TIMER0+i, NULL);
          m_nextTimerEvent[i] = ms + m_timerPeriod[i];
        }          
      }
    }
  }

  // Handlers that sketches must implement  
  virtual void init() = 0;    
  virtual void done() = 0;    
  
  virtual void handleEvent(int iEvent, void *param) = 0;
  
  // Input comes in as MIDI whether it is from Launchpad, other USB hardware or from 
  virtual byte handleInput(byte *msg) = 0;
  
};
