
// inslude the SPI library:
#include <SPI.h>
#include "VNC2SPI.h"
#include "LPDisplay.h"
#include "Playpad.h"
#include "RainstormSketch.h"



VNC2SPI VNC2(10,0);



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
void loop() 
{
  unsigned long newMillis = millis();
  if(lastMillis != newMillis)
    lastMillis = newMillis;
  else
    newMillis = 0;
  Sketch1.run(newMillis);
}

