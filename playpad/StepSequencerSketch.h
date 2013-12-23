class CStepSequencerSketch: 
public CPlaypadSketch
{
  enum {
    NO_SEL = 0xFF,
    NUM_PAGES = 4,
    COL_VEL_HIGH = COutputDriver::LED_RED,
    COL_VEL_MED = COutputDriver::LED_YELLOW,
    COL_VEL_LOW = COutputDriver::LED_GREEN,
    COL_CURSOR = COutputDriver::LED_YELLOW,
    COL_JUMP = COutputDriver::LED_YELLOW,
    COL_START = COutputDriver::LED_GREEN,
    COL_END = COutputDriver::LED_RED    
  };
  byte notes[7];
  unsigned int m_trigger[NUM_PAGES][7];
  byte m_startStep;
  byte m_endStep;
  byte m_currentStep;
  byte m_currentPage;
  byte m_shiftKey;
  byte m_nextStep;
  byte m_running;
  byte m_tickCount;
public:

  ///////////////////////////////////////////////////////////////////////
  void init()
  {
    notes[0] = 42;
    notes[1] = 41;
    notes[2] = 40;
    notes[3] = 39;
    notes[4] = 38;
    notes[5] = 37;
    notes[6] = 36;
    clearPattern();
    m_timerPeriod[0] = 120;
    m_shiftKey = NO_SEL;
    m_nextStep = NO_SEL;
    m_running = 0;
    m_tickCount = 0;
    paint(1);    
  }
  ///////////////////////////////////////////////////////////////////////
  void done()
  {
  }

  void playNote(int which)
  {
    stopNotes();
    midi_note(m_midiChannel, notes[which], 127);
    notes[which]|=0x80;
  }
  void tuneUp(int which)
  {
    stopNotes();
    if(notes[which] < 127)
    {
      ++notes[which];
      midi_note(m_midiChannel, notes[which], 127);
      notes[which]|=0x80;
      for(int i=which-1; i>0; --i)
      {
        if(notes[i]<notes[i+1])
          notes[i] = notes[i+1];
      }
    }           
  }
  void tuneDown(int which)
  {
    stopNotes();
    if(notes[which] > 0)
    {
      --notes[which];
      midi_note(m_midiChannel, notes[which], 127);
      notes[which]|=0x80;
      for(int i=which+1; i<7; ++i)
      {
        if(notes[i]>notes[i-1])
          notes[i] = notes[i-1];
      }
    }           
  }
  void tuneAllUp()
  {
    stopNotes();
    if(notes[0] < 127)
    {
      for(int i=0;i<7;++i)
        ++notes[i];
      midi_note(m_midiChannel, notes[7], 127);
      notes[7]|=0x80;
    }           
  }
  void tuneAllDown()
  {
    stopNotes();
    if(notes[7] > 0)
    {
      for(int i=0;i<7;++i)
        --notes[i];
      midi_note(m_midiChannel, notes[7], 127);
      notes[7]|=0x80;
    }           
  }
  
  void clearPattern()
  {
    stopNotes();
    m_startStep = 0;
    m_endStep = 7;
    m_currentStep = 0;
    m_currentPage = 0;
    memset(m_trigger, 0, sizeof(m_trigger));
  }
  ///////////////////////////////////////////////////////////////////////
  // PAINT THE ENTIRE LOWER PART OF DISPLAY
  void paint(byte clr)
  {
    int x,y;
    if(!m_pOutput)
      return;

    if(clr)
      m_pOutput->cls();
      
    // Show the grid
    for(y=0; y<7; ++y)
    {
      int shift = 14;
      for(x=0; x<8; ++x)
      {
        byte colour = (m_trigger[m_currentPage][y] >> shift) & 0x03;
        switch(colour)
        {
        case 0: 
          m_pOutput->setGrid(y, x, 0); 
          break;
        case 1: 
          m_pOutput->setGrid(y, x, COutputDriver::LED_RED); 
          break;
        case 2: 
          m_pOutput->setGrid(y, x, COutputDriver::LED_YELLOW); 
          break;
        case 3: 
          m_pOutput->setGrid(y, x, COutputDriver::LED_GREEN); 
          break;
        }
        shift-=2;
      }
    }

    // Display cursor row
    int curs = -1;
    int stas = -1;
    int ends = -1;
    if(m_currentPage == m_currentStep/8)
      curs = m_currentStep%8;
    if(m_currentPage == m_startStep/8)
      stas = m_startStep%8;
    if(m_currentPage == m_endStep/8)
      ends = m_endStep%8;
    for(x=0; x<8; ++x)
    {
      if(x == curs)
        m_pOutput->setGrid(7, x, COL_CURSOR);
      else if(x == stas)
        m_pOutput->setGrid(7, x, COL_START);
      else if(x == ends)
        m_pOutput->setGrid(7, x, COL_END);
      else
        m_pOutput->setGrid(7, x, 0);
    }

    // Buttons
    m_pOutput->setGrid(0, 8, m_running ? COutputDriver::LED_GREEN : COutputDriver::LED_RED);
    m_pOutput->setGrid(1, 8, m_running ? 0 : COutputDriver::LED_RED);
    m_pOutput->setGrid(2, 8, (m_shiftKey==2)?COutputDriver::LED_GREEN:0);
    m_pOutput->setGrid(3, 8, (m_shiftKey==3)?COutputDriver::LED_YELLOW:0);
    m_pOutput->setGrid(4, 8, (m_shiftKey==4)?COutputDriver::LED_RED:0);
    m_pOutput->setGrid(5, 8, COutputDriver::LED_GREEN);
    m_pOutput->setGrid(6, 8, COutputDriver::LED_YELLOW);
    m_pOutput->setGrid(7, 8, COutputDriver::LED_RED);


  }


   /////////////////////////////////////////////////////////////////////// 
   void startNotes()
   {
     byte page = m_currentStep/8;
     byte shift = 14 - 2*(m_currentStep%8);
     for(int i=0;i<7;++i)
     {
       byte vel = (m_trigger[page][i] >> shift)&0x03;
       if(vel)
       {
         switch(vel)
         {
           case 1:
             midi_note(m_midiChannel, notes[i]&0x7f, 31);
             break;
           case 2:
             midi_note(m_midiChannel, notes[i]&0x7f, 63);
             break;
           case 3:
             midi_note(m_midiChannel, notes[i]&0x7f, 127);
             break;
         }
         notes[i]|=0x80;
       }
     }
   }

   /////////////////////////////////////////////////////////////////////// 
   void stopNotes()
   {
     for(int i=0;i<7;++i)
     {
       if(notes[i]&0x80)
       {
         midi_note(m_midiChannel, notes[i]&0x7f, 0);
         notes[i]&=~0x80;
       }
     }
   }

  ///////////////////////////////////////////////////////////////////////
  byte handleInput(byte *msg)
  {    
    if(!(INPUT_CH_LPA == (msg[0] & 0x0F) || INPUT_CH_LPB == (msg[0] & 0x0F)))
      return 0;

    int i;
    CLaunchPadInput Input(msg);      
    switch(Input.type)
    {
      case CLaunchPadInput::BUTTON_DOWN:
      if(Input.col == 8)
      {
        switch(Input.row)
        {
          case 0:
            if(m_shiftKey == 1 && !m_running)
              clearPattern();          
            else
              m_running = !m_running;
            paint(0);
            break;
         case 1: 
            m_currentStep = 0;
            m_tickCount = 0;
            m_shiftKey = Input.row;
            break;
         case 2: case 3: case 4:
            m_shiftKey = Input.row;
            paint(0);
            break;
         default:
            m_shiftKey = Input.row;
            break;
        }
      }
      else       
      {
        if(Input.row==7)
        {
          i = Input.col + 8 * m_currentPage;
          if(m_shiftKey == 5 && i < m_endStep)
          {
            m_startStep = i;
            paint(0);
          }          
          else if(m_shiftKey == 6)
          {
            m_nextStep = i;
          }
          else if(m_shiftKey == 7 && i > m_startStep)
          {
            m_endStep = i;
            paint(0);
          }
          else if(m_shiftKey == 2)
          {
            tuneAllUp();
          }
          else if(m_shiftKey == 3)
          {
            playNote(7);
          }
          else if(m_shiftKey == 4)
          {
            tuneAllDown();
          }
          else if(Input.col == 0 && m_currentPage > 0)
          {
            m_currentPage--;
            paint(1);
          }
          else if(Input.col == 7 && m_currentPage < NUM_PAGES - 1)
          {
            m_currentPage++;
            paint(1);
          }                    
        }
        else if(m_shiftKey == 2)
        {
          tuneUp(Input.row);
        }
        else if(m_shiftKey == 3)
        {
          playNote(Input.row);
        }
        else if(m_shiftKey == 4)
        {
          tuneDown(Input.row);
        }
        else
        {
          int s = 8*(int)m_currentPage + Input.col;
          int shift = 14 - 2*Input.col;         
          byte velSelect = 8-m_shiftKey;
          if(velSelect > 0 && velSelect < 3)
          {
            if((m_trigger[m_currentPage][Input.row] & (0x03<<shift)) == (velSelect<<shift))
            {
              m_trigger[m_currentPage][Input.row] &=~(0x03<<shift);           
            }
            else
            {
              m_trigger[m_currentPage][Input.row] &=~(0x03<<shift);
              m_trigger[m_currentPage][Input.row] |= (velSelect<<shift);
            }
          }
          else
          {
            if(m_trigger[m_currentPage][Input.row] & (0x03<<shift))
              m_trigger[m_currentPage][Input.row] &=~(0x03<<shift);
            else
              m_trigger[m_currentPage][Input.row] |= (0x03<<shift);
          }
          paint(0);
        }
      }
      break;
      case CLaunchPadInput::BUTTON_UP:
      if(Input.col == 8)
      {
        switch(m_shiftKey)
        {
          case 2: case 3: case 4:
            m_shiftKey = NO_SEL;
            paint(0);
            break;
          default:
            m_shiftKey = NO_SEL;
            break;
        }
      }
      break;
    }
  }

  ///////////////////////////////////////////////////////////////////////
  void handleEvent(int iEvent, void *param)
  {
    int i;
    switch(iEvent)
    {
    case EV_TICK:
      ++m_tickCount;
      if(m_tickCount >= 6)
      {
        m_tickCount = 0;
        stopNotes();
        if(m_running)
        {
          if(m_nextStep != NO_SEL)
          {
            m_currentStep = m_nextStep;
            m_nextStep = NO_SEL;
          }
          else        
          {
            ++m_currentStep;
          }
          if(m_currentStep > m_endStep)
            m_currentStep = m_startStep;
          else if(m_currentStep < m_startStep)
            m_currentStep = m_startStep;
          startNotes();
          paint(0);
        }
      }
      break;   
    }    
  }
};

