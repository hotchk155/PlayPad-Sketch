class CRainstormSketch: public CPlaypadSketch
{
  byte midiChannel;
  byte notes[8];
  byte trigger[8];
  byte recycle;
public:
  void init(COutputDriver *pOutput)
  {
    midiChannel = 0;
    memset(trigger,0,8);
    recycle = 0;
    for(int i=0; i<8; ++i)
      notes[i] = i+36;
    m_timerPeriod[0] = 500;
    pOutput->cls();
    paint(pOutput);
  }
  void done()
  {
  }
  void stopNotes()
  {
    for(int i=0; i<8; ++i)
    {
      if(notes[i] & 0x80)
      {
        notes[i] &= ~0x80;
        midi_note(midiChannel, notes[i], 0);
      }
    }
  }
  void playNotes(byte trig)
  {
    byte mask = 0x80;
    for(int i=0; i<8; ++i)
    {
      if(trig & mask)
      {
        notes[i] |= 0x80;
        midi_note(midiChannel, notes[i], 127);
      }
      mask>>=1;
    }
  }
  void paint(COutputDriver *pOutput)
  {
    int x,y;
    byte mask;
    for(y=0; y<7; ++y)
    {
      mask = 0x80;
      for(x=0;x<8;++x)
      {
        pOutput->setGrid(y, x, (trigger[y]&mask)? COutputDriver::LED_YELLOW : 0);
        mask>>=1;
      }
    }
    mask = 0x80;
    for(x=0;x<8;++x)
    {
      pOutput->setGrid(7, x, (recycle&mask)? COutputDriver::LED_GREEN : COutputDriver::LED_RED);
      mask>>=1;
    }
  }
  void handleInput(byte *msg, COutputDriver *pOutput)
  {
    
    // Handle this event as Launchpad
    CLaunchPadInput Input(msg);
      
    switch(Input.type)
    {
     case CLaunchPadInput::BUTTON_DOWN:
       if(Input.row<7)
       {
         trigger[Input.row] |= (0x80 >> Input.col);
       }
       else
       {
         recycle ^= (0x80 >> Input.col);         
       }
       paint(pOutput);
       break;
    }
  }
  void handleEvent(int iEvent, void *param, COutputDriver *pOutput)
  {
    int i;
    switch(iEvent)
    {
      case EV_TIMER0:
         {
           stopNotes();
           byte firstRow = trigger[7]&recycle;
           for(i=7; i>0; --i)
             trigger[i] = trigger[i-1];
           trigger[0] = firstRow;           
           playNotes(trigger[7]);
           paint(pOutput);
         }
         break;
    }
  }
};
