
// inslude the SPI library:
#include <SPI.h>


// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;

void setup() {
  // set the slaveSelectPin as an output:
  pinMode (slaveSelectPin, OUTPUT);
  pinMode(2,INPUT);
  // initialize SPI:
  SPI.begin(); 
  Serial.begin(9600);
}

byte row=0;
byte col=0;
byte v = 0x7f;
void loop() {
  byte msg[4];
  if(4==ftdi_read(0,msg,4))
  {
    Serial.print(msg[0],HEX);     Serial.print(",");
    Serial.print(msg[1],HEX);     Serial.print(",");
    Serial.print(msg[2],HEX);     Serial.print(",");
    Serial.print(msg[3],HEX);     Serial.println();
    ftdi_write(0,msg,4);
  }
  /*
  byte msg[4] = {0x01, 0x90, row<<4|col, v};
  ftdi_write(0,msg,4);
  if(col++>8) 
  {
     col = 0;
     if(row++>7)
     {
       row = 0;
       if(v) v=0; else v=0x7f;
     }
  }
  delay(20);
  */
  /*
  // take the SS pin low to select the chip:
  digitalWrite(slaveSelectPin,LOW);
  //  send in the address and value via SPI:
  Serial.println("---");
  Serial.print(SPI.transfer(0xff), DEC);
  Serial.print(" ");
  Serial.print(SPI.transfer(0x01), DEC);
  Serial.print(" ");
  Serial.print(SPI.transfer(0x90), DEC);
  Serial.print(" ");
  Serial.print(SPI.transfer(0x11), DEC);
  Serial.print(" ");
  Serial.println(SPI.transfer(0x7f), DEC);
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin,HIGH); 
  */

}
#define SLAVE_SELECT(v) digitalWrite(slaveSelectPin,v)
#define FTDI_WRITE 0x00
#define FTDI_READ 0x10
#define FTDI_TXE 0x08
#define FTDI_RXF 0x04
#define FTDI_ACK 0x02

int ftdi_write(byte addr, byte *pData, int iLen)
{
  byte command = (addr<<5)|FTDI_WRITE;
  SLAVE_SELECT(LOW);  
  delayMicroseconds(1);
  byte s = SPI.transfer(command); 
  int p;
  for(p=0; p<iLen && !(s&FTDI_RXF);p++)
    s = SPI.transfer(pData[p]);
  delayMicroseconds(1);
  SLAVE_SELECT(HIGH);  
  return p;
}

byte ftdi_read(byte addr, byte *pData, int iLen)
{
  byte command = (addr<<5)|FTDI_READ;
  SLAVE_SELECT(LOW);  
  delayMicroseconds(1);  
  byte s = SPI.transfer(command); // Command byte for write address 0
  int p;
  for(p=0; p<iLen && FTDI_ACK == (s&(FTDI_TXE|FTDI_ACK));p++)
  {
    pData[p] = SPI.transfer(0);
    s = SPI.transfer(0);
  }
  delayMicroseconds(1);    
  SLAVE_SELECT(HIGH);  
  return p;  
}
