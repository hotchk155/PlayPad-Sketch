class CEchoTest: public CPlaypadSketch
{
public:
  void init()
  {
  }
  void done()
  {
  }
  void handleInput(byte *msg, COutputDriver *pOutput)
  {
    if(!pOutput)
      return;
    
    midi_out(msg);
    
    // Handle this event as Launchpad
    CLaunchPadInput Input(msg);
      
    switch(Input.type)
    {
     case CLaunchPadInput::BUTTON_DOWN:
        pOutput->setGrid(Input.row, Input.col, COutputDriver::LED_YELLOW);
        break;
     case CLaunchPadInput::BUTTON_UP:
        pOutput->setGrid(Input.row, Input.col, 0);
        break;
    }
  }
  void handleEvent(int iEvent, void *param, COutputDriver *pOutput)
  {
  }
};
