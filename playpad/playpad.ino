
// inslude the SPI library:
#include <SPI.h>
#include "VNC2SPI.h"
#include "LaunchpadInput.h"
#include "OutputDriver.h"
#include "PlaypadSketch.h"
#include "EchoTest.h"



#define VNC2_SS_PIN 10
VNC2SPI VNC2(VNC2_SS_PIN);



void midi_note(byte chan, byte note, byte vel)
{
  Serial.write(0x90|chan);
  Serial.write(note & 0x7f);
  Serial.write(vel & 0x7f);
}



unsigned long lastMillis;


  #define SZ_BUFFER 200    

#define MAX_SKETCHES 4
CPlaypadSketch* Sketches[MAX_SKETCHES];
byte runningSketches = 0;
byte output1Owner = 0;
byte output2Owner = 0;


COutputDriver Output1;
COutputDriver Output2;

byte inputMsg[3];
byte inputParam1 = 0;

void initSketches()
{
  runningSketches = 0;
  output1Owner = 0;
  output2Owner = 0;
  for(int i=0; i<MAX_SKETCHES; i++)
    Sketches[i] = NULL;
}

void setup() 
{
  int i;
  // initialize SPI:
  Serial.begin(9600);
  Serial.flush();  
  VNC2.begin();
  initSketches();
  
  Sketches[runningSketches++] = new CEchoTest();
}



void loop() 
{
    
  byte buffer[SZ_BUFFER];  
  int count, i, j;
 
  // Check whether the output devices have any
  // buffered data to send
  count = Output1.getData(buffer, 0, SZ_BUFFER);
  if(count) VNC2.write(0, buffer, count);
  count = Output2.getData(buffer, 1, SZ_BUFFER);
  if(count) VNC2.write(1, buffer, count);
         
   // read activity from USB. The messages use
   // MIDI style presentation.
   count = VNC2.read(0,buffer,SZ_BUFFER);
   for(i=0; i<count; ++i)
   {
       Serial.print("0x");
       Serial.print(buffer[i], HEX);
       Serial.print(" ");
       
     if(buffer[i]&0x80) // start of new message
     {
       inputMsg[0] = buffer[i];
       inputParam1 = 1;
     }
     else if(inputParam1)
     {
       inputMsg[1] = buffer[i];
       inputParam1 = 0;
     }
     else 
     {
       inputMsg[2] = buffer[i];
       inputParam1 = 1;       
       if(output1Owner == output2Owner)
       {
         if(output1Owner >= 0) Sketches[output1Owner]->handleInput(inputMsg, &Output1, &Output2); 
       }
       else 
       {
         if(output1Owner >= 0) Sketches[output1Owner]->handleInput(inputMsg, &Output1, NULL); 
         if(output2Owner >= 0) Sketches[output2Owner]->handleInput(inputMsg, &Output2, NULL); 
       }
     }
  }
    
  // Every 1ms we call the handleTimer method of each sketch driver, passing
  // it the zero, one or two output devices that are currently "owned" by the
  // sketch driver
  unsigned long newMillis = millis();
  if(lastMillis != newMillis)
  {
    lastMillis = newMillis;
    for(j=0;j<runningSketches;++j)
    {
       if(j==output1Owner)
       {
         if(j==output2Owner)
           Sketches[j]->handleTimer(newMillis, &Output1, &Output2); 
         else
           Sketches[j]->handleTimer(newMillis, &Output1, NULL); 
       }
       else if(j==output2Owner)
         Sketches[j]->handleTimer(newMillis, &Output2, NULL); 
       else
         Sketches[j]->handleTimer(newMillis, NULL, NULL); 
      }
    }
  }


