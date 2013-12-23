
// inslude the SPI library:
#include <SPI.h>

void midi_out(byte *msg);
void midi_note(byte chan, byte note, byte vel);

#include "VNC2SPI.h"
#include "LaunchpadInput.h"
#include "OutputDriver.h"
#include "PlaypadSketch.h"
//#include "EchoTest.h"
//#include "RainstormSketch.h"
#include "StepSequencerSketch.h"

#define MIDI_TICK     0xF8
#define MIDI_START    0xFA
#define MIDI_CONTINUE 0xFB
#define MIDI_STOP     0xFC


#define VNC2_SS_PIN 10
VNC2SPI VNC2(VNC2_SS_PIN);

#define LED_PIN  8

void midi_out(byte *msg)
{
  Serial.write(msg[0]);
  Serial.write(msg[1]);
  Serial.write(msg[2]);
  
}
void midi_note(byte chan, byte note, byte vel)
{
  Serial.write(0x90|chan);
  Serial.write(note & 0x7f);
  Serial.write(vel & 0x7f);
}



unsigned long lastMillis;


  #define SZ_BUFFER 100    

/*
#define MAX_SKETCHES 4
CPlaypadSketch* Sketches[MAX_SKETCHES];
byte runningSketches = 0;
byte output1Owner = 0;
byte output2Owner = 0;
*/

CPlaypadSketch *pRunningSketch = NULL;
COutputDriver OutputDriver;
//COutputDriver Output2;

byte inputMsg[3];
byte inputParam1 = 0;

byte midiMsg[3];
byte midiParam1 = 0;
byte midiNumParams = 0;
byte midiClockRunning = 0;

void initSketches()
{
  midiClockRunning = 1;
//  runningSketches = 0;
//  output1Owner = 0;
//  output2Owner = 0;
//  for(int i=0; i<MAX_SKETCHES; i++)
//    Sketches[i] = NULL;
}

void dispatchEvent(int iEvent, void *param = 0)
{
  pRunningSketch->handleEvent(iEvent, param);
}

// Check for MIDI data on the main MIDI input port
byte receiveMIDI()
{
  while(Serial.available())
  {	
    byte q = Serial.read();
    if((q&0xF0) == 0xF0)      // realtime message
    {
      switch(q)
      {
        case MIDI_TICK:
          if(midiClockRunning)
            dispatchEvent(CPlaypadSketch::EV_TICK, NULL);
          break;
        case MIDI_START:
          midiClockRunning = 1;
          dispatchEvent(CPlaypadSketch::EV_RESTART, NULL);
          break;
        case MIDI_CONTINUE:
          midiClockRunning = 1;
          break;
        case MIDI_STOP:
          midiClockRunning = 0;          
          break;
      }
    }
    else if(q&0x80)  // status message on any channel
    {
      midiMsg[0] = q;
      switch(q&0xF0)
      {
        case 0x80: //  Note-off  2  key  velocity  
	case 0x90: //  Note-on  2  key  veolcity  
	case 0xA0: //  Aftertouch  2  key  touch  
	case 0xB0: //  Continuous controller  2  controller #  controller value  
	case 0xC0: //  Patch change  2  instrument #   
	case 0xE0: //  Pitch bend  2  lsb (7 bits)  msb (7 bits)  
	  midiNumParams = 2;
	  midiParam1 = 1;
	  break;
        case 0xD0: //  Channel Pressure  1  pressure  
	  midiNumParams = 2;
	  midiParam1 = 1;
  	  break;
      }
    }
    else if(midiParam1)  // first message param
    {
      midiMsg[1] = q;
      if(midiNumParams == 2)  
        midiParam1 = 0;  // need a second param
      else
        return 1;  // no second param needed - we have whole message to return
    }
    else
    {
      midiMsg[2] = q;  // got second param
      midiParam1 = 1;  // next time we'll be after the first param
      return 1; // and we have a message
    } 
  }
  return 0; // no message right now
}  

void setup() 
{
  int i;
  // initialize SPI:
  Serial.begin(31250);
  Serial.flush();  
  VNC2.begin();
//  initSketches();
  pinMode(LED_PIN,OUTPUT);
  OutputDriver.init();
//  pRunningSketch = new CEchoTest();
  pRunningSketch = new CStepSequencerSketch();
  pRunningSketch->setMidiChannel(0);
  pRunningSketch->setOutput(&OutputDriver);
  pRunningSketch->init();
}



void handleInput(byte *msg)
{
  switch(msg[0]&0x0F)
  {
    case CPlaypadSketch::INPUT_CH_LPA:
    case CPlaypadSketch::INPUT_CH_LPB:
      {
        CLaunchPadInput Input(msg);
        if(Input.row == 8 && Input.type == CLaunchPadInput::BUTTON_DOWN)
        {
          switch(Input.col)
          {
            case 2: pRunningSketch->handleEvent(CPlaypadSketch::EV_ARROWLEFT, NULL); break;
            case 3: pRunningSketch->handleEvent(CPlaypadSketch::EV_ARROWRIGHT, NULL); break;
          }
        }
        else
        {
          pRunningSketch->handleInput(inputMsg); 
        }
      }
      break;
    default:
      pRunningSketch->handleInput(inputMsg); 
      break;
  }
}

void loop() 
{
    
  unsigned long newMillis = millis();
  byte buffer[SZ_BUFFER];  
  int count, i, j;
 
   // read activity from USB. The messages use MIDI style presentation   
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
       inputParam1 = 0;
     }
     else 
     {
       inputMsg[2] = buffer[i];
       inputParam1 = 1;              
       handleInput(inputMsg); 
     }
  }
  
  // Check for input at the MIDI port
  if(receiveMIDI())
  {
    digitalWrite(LED_PIN,HIGH); 
    midiMsg[0] &= 0xF0;
    midiMsg[0] |= CPlaypadSketch::INPUT_CH_MIDI; // change source channel to indicate input port
    pRunningSketch->handleInput(midiMsg);
    digitalWrite(LED_PIN,LOW); 
  }
    
  // Every 1ms we call the handleTimer method of each sketch driver, passing
  // it the zero, one or two output devices that are currently "owned" by the
  // sketch driver
  if(lastMillis != newMillis)
  {
    lastMillis = newMillis;
    pRunningSketch->handleTimer(newMillis);
  }

  // Check whether the output devices have any
  // buffered data to send
  count = OutputDriver.getData(buffer, 0, SZ_BUFFER);
  if(count) VNC2.write(0, buffer, count);
}
