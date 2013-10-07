class CEchoTest: public IPlaypadSketch
{
  CSketchDriver *m_pDriver;
public:
  void init(CSketchDriver *pDriver)
  {
    m_pDriver = pDriver;
  }
  void done()
  {
  }
  void handleEvent(int event, int param1, int param2)
  {
    switch(event)
    {
     case EV_GRID_PRESS:
        m_pDriver->m_pLP->setLed(LP_GRID(param1,param2),LP_YELLOW);
        break;
     case EV_GRID_RELEASE:
        m_pDriver->m_pLP->setLed(LP_GRID(param1,param2),0);
        break;
     case EV_RIGHT_PRESS:
        m_pDriver->m_pLP->setLed(LP_RIGHT(param1),LP_YELLOW);
        break;
     case EV_RIGHT_RELEASE:
        m_pDriver->m_pLP->setLed(LP_RIGHT(param1),0);
        break;
     case EV_TOP_PRESS:
        m_pDriver->m_pLP->setLed(LP_TOP(param1),LP_YELLOW);
        break;
     case EV_TOP_RELEASE:
        m_pDriver->m_pLP->setLed(LP_TOP(param1),0);
        break;
    }
  }
};
