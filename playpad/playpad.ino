
// inslude the SPI library:
#include <SPI.h>
#include "LPDisplay.h"
#include "Playpad.h"
#include "RainstormSketch.h"



// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;
byte ftdi_status = 0;

#define SLAVE_SELECT(v) digitalWrite(slaveSelectPin,v)
#define FTDI_WRITE 0x00
#define FTDI_READ 0x10
#define FTDI_TXE 0x08
#define FTDI_RXF 0x04
#define FTDI_ACK 0x02



void midi_note(byte chan, byte note, byte vel)
{
  Serial.write(0x90|chan);
  Serial.write(note & 0x7f);
  Serial.write(vel & 0x7f);
}

/*
int ftdi_write(byte addr, byte *pData, int iLen)
{
  byte command = (addr<<5)|FTDI_WRITE;
  SLAVE_SELECT(LOW);  
  delayMicroseconds(1);
  ftdi_status = SPI.transfer(command); 
  int p;
  for(p=0; p<iLen;p++)
  {
    ftdi_status = SPI.transfer(pData[p]);
  }
  delayMicroseconds(1);
  SLAVE_SELECT(HIGH);  
  return p;
}
*/

void ftdi_write(byte addr, byte *pData, int iLen)
{
  int p=0;
  byte command = (addr<<5)|FTDI_WRITE;
  while(p<iLen)
  {    
      for(;;)
      {
        SLAVE_SELECT(LOW);  
        delayMicroseconds(1);
        ftdi_status = SPI.transfer(command); 
        Serial.print("q");
        if(!(ftdi_status & FTDI_RXF))
          break;
        Serial.print("f");          
        delayMicroseconds(1);
        SLAVE_SELECT(HIGH);  
        delay(5);
      }
        
      while(p<iLen)
      {  
        ftdi_status = SPI.transfer(pData[p]);
        Serial.print("x");
        p++; // byte sent?
        if(ftdi_status & FTDI_RXF)
          break;        
      }
  }
        Serial.print("y");  
  delayMicroseconds(1);
  SLAVE_SELECT(HIGH);  
}

int ftdi_read(byte addr, byte *pData, int iLen)
{
  byte command = (addr<<5)|FTDI_READ;
  SLAVE_SELECT(LOW);  
  delayMicroseconds(1);  
  ftdi_status = SPI.transfer(command); // Command byte for write address 0
  int p;
  for(p=0; p<iLen && FTDI_ACK == (ftdi_status&(FTDI_TXE|FTDI_ACK));p++)
  {
    pData[p] = SPI.transfer(0);
    ftdi_status = SPI.transfer(0);
  }
  delayMicroseconds(1);    
  SLAVE_SELECT(HIGH);  
  return p;  
}





LPDisplay LPA(0);
CSketchDriver Sketch1;

void setup() {
  
  // set the slaveSelectPin as an output:
  pinMode (slaveSelectPin, OUTPUT);
  pinMode(2,INPUT);
  // initialize SPI:
  Serial.begin(9600);
  Serial.flush();
  
  Serial.print("Hello");
  SPI.begin(); 
  
  Sketch1.m_pLP = &LPA;
  Sketch1.m_pSketch = new CRainstormSketch;
  Sketch1.m_pSketch->init(&Sketch1);
}


unsigned long lastMillis;
void loop() 
{
  unsigned long newMillis = millis();
  if(lastMillis != newMillis)
    lastMillis = newMillis;
  else
    newMillis = 0;
  Sketch1.run(newMillis);
}

