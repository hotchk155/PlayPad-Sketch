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
Serial.print("p:");     
        m_pDriver->m_pLP->setLed(LP_GRID(param1,param2),LP_YELLOW);
        break;
     case EV_GRID_RELEASE:
Serial.print("q");     
        m_pDriver->m_pLP->setLed(LP_GRID(param1,param2),0);
        break;
     case EV_RIGHT_PRESS:
Serial.print("r");
        m_pDriver->m_pLP->setLed(LP_RIGHT(param1),LP_YELLOW);
        break;
     case EV_RIGHT_RELEASE:
Serial.print("s");
        m_pDriver->m_pLP->setLed(LP_RIGHT(param1),0);
        break;
     case EV_TOP_PRESS:
Serial.print("t");
        m_pDriver->m_pLP->setLed(LP_TOP(param1),LP_YELLOW);
        break;
     case EV_TOP_RELEASE:
Serial.print("u");
        m_pDriver->m_pLP->setLed(LP_TOP(param1),0);
        break;
    }
Serial.print(param1,DEC);     
Serial.print(",");     
Serial.print(param2,DEC);     
Serial.print(";");     
  }
};