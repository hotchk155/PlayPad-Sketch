//
// Convenience class for handling input messages that
// come from Launchpad
//
// port = 0..1 for message source
// row = 0..7 for normal rows, 8 for menu buttons
// col = 0..7 for normal rows, 8 for arrow buttons
class CLaunchPadInput
{  
public:
  enum {
    BUTTON_DOWN = 1,
    BUTTON_UP = 2
  };
  byte port;
  byte type;
  byte row;
  byte col;
  CLaunchPadInput(byte *msg)
  {
    port = (msg[0] & 0x0F);
    if((msg[0] & 0xF0) == 0xB0)
    {
      row = 8;
      if(msg[1] > 0x68)
        col = ((msg[1] - 0x68)&0x07);
      else
        col = 0;
    }
    else
    {
      row = msg[1]>>4;
      col = msg[1]&0x0F;
    }
    type = msg[2]? BUTTON_DOWN : BUTTON_UP;
  }
};

