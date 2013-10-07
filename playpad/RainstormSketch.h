class CRainstormSketch : public IPlaypadSketch
{
  byte crap;
  byte grid[8];
  CSketchDriver *m_pDriver;
public:
  void init(CSketchDriver *pDriver)
  {
    m_pDriver->m_pLP->clearAll();
    m_pDriver = pDriver;
    for(int i=0;i<8;++i)
    {
      LP_SET(7,i,LP_GREEN);
    }
    memset(grid,0,8);
    m_pDriver->m_Timer1 = 500;
    crap=0;
  }  
   void done()
   {
   }
   void handleEvent(int event, int param1, int param2)
   {
     byte mask;
     int i,j;
     switch(event)
     {
       case EV_GRID_PRESS:
         if(param1 == 7)
         {
           LP_SET(param1,param2, (LP_GET(param1,param2) == LP_RED)? LP_GREEN : LP_RED);
         }
         else
         {
           mask = 1<<(7-param2);
           grid[param1] ^= mask;
           LP_SET(param1,param2,(grid[param1]&mask)? LP_YELLOW:0);
         }
         break;
       case EV_TIMER1:
         byte carry = grid[7];         
         grid[0] = crap; 
         crap=!crap;
         for(i=7; i>0;--i)         
           grid[i]=grid[i-1];                   
         grid[0]=carry;
         for(i=0; i<7; ++i)
         {         
           mask = 0x80;
           for(j=0; j<8;++j)
           {
            LP_SET(i,j,(grid[i]&mask)? LP_YELLOW:0);
            mask >>= 1;
           }
         }
         break;
     }
   }
};
