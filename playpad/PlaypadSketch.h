/////////////////////////////////////////////////////
//
// Based class and interface for "sketches" that run
// on the hib
//
/////////////////////////////////////////////////////
class CPlaypadSketch
{  
  enum { MAX_TIMERS = 4 };
  unsigned long m_nextTimerEvent[MAX_TIMERS];
  unsigned int m_timerPeriod[MAX_TIMERS];
  

public:  
  enum {
    EV_TIMER0 = 100
  };

  CPlaypadSketch()
  {
    memset(m_nextTimerEvent, 0, sizeof(m_nextTimerEvent));
    memset(m_timerPeriod, 0, sizeof(m_timerPeriod));
  }
       
  // trigger timer events
  void handleTimer(unsigned long ms, COutputDriver *pOutput1, COutputDriver *pOutput2)
  {
    for(int i=0;i<MAX_TIMERS;++i)
    {
      if(m_timerPeriod[i])
      {
        if(m_nextTimerEvent[i] < ms)
        {
          handleEvent(EV_TIMER0+i, NULL, pOutput1, pOutput2);
          m_nextTimerEvent[i] = ms + m_timerPeriod[i];
        }          
      }
    }
  }

  // Handlers that sketches must implement  
  virtual void init() = 0;    
  virtual void done() = 0;    
  virtual void handleEvent(int iEvent, void *param, COutputDriver *pOutput1, COutputDriver *pOutput2) = 0;
  virtual void handleInput(byte *msg, COutputDriver *pOutput1, COutputDriver *pOutput2) = 0;
  
};
