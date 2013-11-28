// Currently the output is specific to Launchpad
class COutputDriver
{
public:  
  enum { 
    CELL_MAX = 80,
    CELL_LED_MASK = 0x33,
    CELL_DIRTY = 0x80
  };
  enum {
      LED_RED      = 0x03, 
      LED_RED1     = 0x01, 
      LED_RED2     = 0x02,
      LED_GREEN    = 0x30,
      LED_GREEN1   = 0x10,
      LED_GREEN2   = 0x20,
      LED_YELLOW   = 0x33,
      LED_YELLOW1  = 0x11,
      LED_YELLOW2  = 0x22
  };

   byte m_status;
   byte m_ledStatus[LP_LED_MAX];
public:
   COutputDriver()
   {
     memset(m_ledStatus, 0, sizeof(m_ledStatus));
   }
   
   void setGrid(byte row, byte col, byte value) 
   {
     unsigned int index = row*9+col;
     if(index < CELL_MAX)
     {
       if(value != (m_ledStatus[index]&CELL_LED_MASK)))
       {
         m_ledStatus[index] = value|CELL_DIRTY;
         m_status|=CELL_DIRTY;
       }
     }
   }

   byte getGrid(byte row, byte col) 
   {
     unsigned int index = row*9+col;
     if(index < CELL_MAX)
       return m_ledStatus[index]&CELL_LED_MASK);
     return 0;
   }
  
   int getData(byte *buffer, byte port, int count)
   {
     if(!(m_status&CELL_DIRTY))
       return 0; // nothing to send

     int addr=0;     
     int maxAddr = count - 3;
     int index = 0;
     int row;
     int col;
     byte isFirst = 1;          
     for(row = 0; row < 8; ++row)
     {
       for(col = 0; col < 9; ++col)
       {         
         if(m_ledStatus[index]&CELL_DIRTY)
         {
            if(isFirst) 
            {
              buffer[addr++] = (0x90|port);          
              isFirst=0;
            }
            buffer[addr++] = row<<4|col;
            buffer[addr++] = (m_ledStatus[index]&CELL_LED_MASK);
            m_ledStatus[index] &= ~CELL_DIRTY;
            if(addr >= maxAddr)
               return addr;
         }
         ++index;
     }
     isFirst=1;
     for(col = 0x68; col < 0x70; ++col)
     {         
       if(m_ledStatus[index]&CELL_DIRTY)
       {
          if(isFirst) 
          {
            buffer[addr++] = (0xB0|port);          
            isFirst=0;
          }
          buffer[addr++] = col
          buffer[addr++] = (m_ledStatus[index]&CELL_LED_MASK);
          m_ledStatus[index] &= ~CELL_DIRTY;
          if(addr >= maxAddr)
            return addr;
       }
       ++index;
     }
     m_status&=~LP_STATUS_DIRTY;
     return addr;
   }   
};

