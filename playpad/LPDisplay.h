


#define LP_RED      0x03
#define LP_RED1     0x01
#define LP_RED2     0x02

#define LP_GREEN    0x30
#define LP_GREEN1   0x10
#define LP_GREEN2   0x20

#define LP_YELLOW   0x33
#define LP_YELLOW1  0x11
#define LP_YELLOW2  0x22

#define LP_RIGHT_OFS  64
#define LP_TOP_OFS    72
#define LP_LED_MAX    80

#define LP_STATUS_DIRTY        0x80
#define LP_STATUS_COLOUR_MASK  0x7f

#define LP_GRID(y,x)  ((y)<<3|(x))
#define LP_TOP(x)      (LP_TOP_OFS+(x))
#define LP_RIGHT(y)    (LP_RIGHT_OFS+(y))

#define LP_SET(row,col,val) m_pDriver->m_pLP->setLed(LP_GRID((row),(col)), val)
#define LP_GET(row,col) m_pDriver->m_pLP->getLed(LP_GRID((row),(col)))

#define TAG_PORTA 0x40
#define TAG_PORTB 0x80

class LPDisplay
{
   byte m_chan;
   byte m_status;
   byte m_ledStatus[LP_LED_MAX];
public:
   LPDisplay(byte chan)
   {
     memset(m_ledStatus, 0, LP_LED_MAX);
     m_status = 0;
     m_chan = chan;
   }
   

   byte clearAll() 
   {
    // for(int i=0;i<LP_LED_MAX;++i)
      // m_ledStatus[i] = LP_STATUS_DIRTY;
//      m_status|=LP_STATUS_DIRTY;
   }
   void setLed(byte led, byte col) 
   {
     if((led < LP_LED_MAX) && (col != (m_ledStatus[led]&LP_STATUS_COLOUR_MASK)))
     {
       m_ledStatus[led] = col|LP_STATUS_DIRTY;
       m_status|=LP_STATUS_DIRTY;
     }
   }

   byte getLed(byte led) 
   {
     if(led < LP_LED_MAX)
       return m_ledStatus[led]&LP_STATUS_COLOUR_MASK;
   }

  
   byte _getData(byte *buffer)
   {
     int i;
     int addr=0;
     byte isFirst;
     if(!(m_status&LP_STATUS_DIRTY))
       return 0; // nothing to send
          
     isFirst = 1;
     for(i=0;i<LP_RIGHT_OFS;i++)
     {
       if(m_ledStatus[i]&LP_STATUS_DIRTY)
       {
          if(isFirst) 
          {
            buffer[addr++] = (0x90|m_chan);          
            isFirst=0;
          }
          buffer[addr++] = ((i/8)<<4)|(i&0x07);
          buffer[addr++] = (m_ledStatus[i] & LP_STATUS_COLOUR_MASK);
          m_ledStatus[i] &= ~LP_STATUS_DIRTY;
       }
     }
     for(;i<LP_TOP_OFS;i++)
     {
       if(m_ledStatus[i]&LP_STATUS_DIRTY)
       {
          if(isFirst) 
          {
            buffer[addr++] = (0x90|m_chan);          
            isFirst=0;
          }
          buffer[addr++] = ((i-LP_RIGHT_OFS)<<4)|0x08;
          buffer[addr++] = (m_ledStatus[i] & LP_STATUS_COLOUR_MASK);
          m_ledStatus[i] &= ~LP_STATUS_DIRTY;
       }
     }
     isFirst=1;
     for(;i<LP_LED_MAX;i++)
     {
       if(m_ledStatus[i]&LP_STATUS_DIRTY)
       {
          if(isFirst) 
          {
            buffer[addr++] = (0xB0|m_chan);          
            isFirst=0;
          }
          buffer[addr++] = 0x68 + i-LP_TOP_OFS;
          m_ledStatus[i] &= ~LP_STATUS_DIRTY;
       }
     }
     
     m_status&=~LP_STATUS_DIRTY;
     return addr;
   }   
};

/*
LPDisplay


enum 
{
  SPI_STATE_IDLE,
  SPI_STATE_SENDCMD,
  SPI_STATE_SENDDATA
}

#define SPI_EXPECT_SLAVE_STATUS  0x01
#define SPI_

enum 
{
  SPI_TX_COMMAND,
  SPI_TX_STATUS,
  SPI_TX_DATA,
  SPI_RX_COMMAND,
  SPI_RX_STATUS,
  SPI_RX_DATA,
}
///////////////////////////////////////////////////////
// Interrupt service routine is called each time the 
// SPI peripheral has finished sending a byte
ISR (SPI_STC_vect)
{
  switch(spi_mode)
  {
  case SPI_TX_DATA:
      spi_slave_status = SPDR;
      if( // slave cannot receive data )
      {
        // send command byte again
        spi_state = SPI_TX_DATA;
      }
      else
      if( // more data to send in the current message, )
      {
        // send next byte
      }
      else 
      if(!(spi_slave_status&FTDI_TXE)) // does slave have something to send?
      {
        // send read command
        spi_mode = SPI_RX_STATUS;
      }
      else 
      {
        spi_mode = SPI_TX_IDLE;
      }
      break;
    case SPI_RX_STATUS:
      spi_slave_status = SPDR;
      if(//slave tx buffer is empty)
      {
         spi_mode = SPI_RX_IDLE;
      }
      else
      {
        // send ZERO
        spi_state = SPI_RX_DATA;
      }
      break;    
    case SPI_RX_DATA:
      // store the received byte
      if(//message complete)
      {
         spi_mode = SPI_RX_IDLE;
      }
      break;
    }
    
  }

  if(spi_state == SPI_RX_IDLE) 
  {
    if(// any new message to send)
    {
      // get ready for it
      // send WRITE command
      spi_state = SPI_TX_DATA;
    }
    else
    {
      // send READ command
      spi_state = SPI_RX_STATUS;
    }
  }
  else 
  if(spi_state == SPI_TX_IDLE) 
  {
    if(// slave wants to send)
    {
      // send READ command
      spi_state = SPI_RX_STATUS;
    }
    else
    {
      // check for data to send from LP
      if(//the is any)
      {
        // get ready for it
        // send WRITE command
        spi_state = SPI_TX_DATA;
      }      
      else
      {
        // send READ command
        spi_state = SPI_RX_STATUS;
      }
    }
  }
  

  if(SPI_MODE_RECEIVE == spi_mode)
  {
    if(SPI_COMMAND == spi_state)
    {
      // send the command
      spi_state = SPI_SLAVE_STATUS;
    }
  }
  
  
  if(spi_status&SPI_EXPECT_SLAVE_STATUS) // expecting receiver status byte  
  {
    spi_receiver_status = SPDR; // store receiver status
    if(spi_status&SPI_TRANSMIT_TO_SLAVE) // actively transmitting
    {
      // send the next byte, reyain SPI_EXPECT_SLAVE_STATUS
    }
    else if(spi_status&SPI_RECEIVE_SLAVE)
    {
      spi_status&=~SPI_EXPECT_SLAVE_STATUS
      SPDR = 0; // clock the next data byte 
    }

    
  }
  else if(spi_status&SPI_READ_FROM_SLAVE) // is this a read operation?
  {
    // store data byte
    if(//end of receive)
    {
    }
    else
    {
      spi_status|=SPI_EXPECT_SLAVE_STATUS; // next received byte will be slave status
      SPDR = 0;  //send a zero to clock next byte from slave
    }
  }
  
  
  
  
  if(spi_data_byte_expected) // we expect to receive a data byte from SPI  
  {
    spi_data_byte_expected = !spi_data_byte_expected; // there is a status byte between bytes
    spi_rx_msg[spi_rx_msg_pos++] = d;
    if(spi_rx_msg_pos == 4)
    {
      // end of message
    }
  }
  else
  {
      // receive status byte
      spi_slave_status = d;
      if(spi_
  

	// are we idle?
	if(sd600_dataPtr < 0) 
	{
		// send a zero - the sd600 needs us to keep clocking the line
		// even when no data is being sent
		SPDR = 0;
	} 
	else 
	{
		// start sending the next data byte
		SPDR = sd600_data[sd600_dataPtr];

		// increment the data pointer, setting it back to -1 for 
		// idle mode when we reach the end of the buffer
		if(++sd600_dataPtr >= sd600_dataLen)
			sd600_dataPtr = -1;
	}
}
*/
