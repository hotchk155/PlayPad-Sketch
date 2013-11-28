
// inslude the SPI library:
#include <SPI.h>
#include "VNC2SPI.h"
#include "LPDisplay.h"
#include "Playpad.h"
#include "RainstormSketch.h"



VNC2SPI VNC2;



void midi_note(byte chan, byte note, byte vel)
{
  Serial.write(0x90|chan);
  Serial.write(note & 0x7f);
  Serial.write(vel & 0x7f);
}



LPDisplay LPA(0);
CSketchDriver Sketch1;

void setup() {
  
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


  #define SZ_BUFFER 200    

#define MAX_SKETCHES 4
ISketchDriver* apSketchDrivers[MAX_SKETCHES];
byte runningSketches = 0;
byte outputAssignments[MAX_SKETCHES];


IOutputDriver *pOutput1 = NULL;
IOutputDriver *pOutput2 = NULL;

ISketchDriver* pOutput1Owner = NULL;
ISketchDriver* pOutput2Owner = NULL;

byte inputMsg[3];
byte inputParam1 = 0;

void loop() 
{
    
  byte buffer[SZ_BUFFER];  
  int count, i, j;
  ISketchDriver *pThisSketchDriver;
  IOutputDriver *p1;
  IOutputDriver *p2;
 
  // Check whether the output devices have any
  // buffered data to send
  if(pOutput1)
  {
    count = pOutput1->getData(buffer, SZ_BUFFER);
    if(count) VNC2.write(0, buffer, count);
  }
  if(pOutput2)
  {
    count = pOutput2->getData(buffer, SZ_BUFFER);
    if(count) VNC2.write(1, buffer, count);
  }
         
   // read activity from USB. The messages use
   // MIDI style presentation.
   count = VNC2.read(0,buffer,SZ_BUFFER);
   for(i=0; i<count; ++i)
   {
     if(buffer[i]&0x80) // start of new message
     {
       inputMsg[0] = buffer[i];
       inputParam1 = 1;
     }
     else if(inputParam1)
     {
       inputMsg[1] = buffer[i];
       inputParam1 = 0
     }
     else 
     {
       midiMsg[2] = buffer[i];
       inputParam = 1;
     
       // pass the message to all running sketches
       for(j=0;j<runningSketches;++j)
       {
         switch(outputAssignments[j])
         {
           case 1: pThisSketchDriver->handleInput(inputMsg, Output1, NULL); break;
           case 2: pThisSketchDriver->handleInput(inputMsg, Output2, NULL); break;
           case 3: pThisSketchDriver->handleInput(inputMsg, Output1, Output2); break;
           default: pThisSketchDriver->handleInput(inputMsg, NULL, NULL); break;
         }
       }
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
      switch(outputAssignments[j])
      {
        case 1: pThisSketchDriver->handleTimer(newMillis, Output1, NULL); break;
        case 2: pThisSketchDriver->handleTimer(newMillis, Output2, NULL); break;
        case 3: pThisSketchDriver->handleTimer(newMillis, Output1, Output2); break;
        default: pThisSketchDriver->handleTimer(newMillis, NULL, NULL); break;
      }
    }
  }
    
  
};

  
  
}





