  // include MIDI library
#include <MIDI.h>

/*
BMPC-LANC-MIDI v1.2 (2014 www.grifu.com)
LANC Remote Control IRIS/FOCOUS for Blackmagic Pocket Camera (BMPC)

LANC to MIDI to be used with BMPC.
Download Arduino MIDI library from http://playground.arduino.cc/Main/MIDILibrary

This sketch listens to the MIDI channel 4 looking for Control Messages that
Will be translated to LANC commands.

This Sketch was made for the special purpose of a puppet play called "Peregrinaçao"
With this sketch we can cantrol the Focus and Iris of 3 cameras using one arduino. 

I use virtual MIDI to be able to control the LANC messages from any software.
Because Arduino uses the serial port we need another application to
read MIDI messages from the serial port and send them to a virtual midi.
Hairless-midi is a free MAC/PC application that reads MIDI from the serial port.

Lanc Commands
FOCUS NEAR/FAR/AUTO
IRIS PLUS/MINUS/AUTO

This code is very draft and could be optimized, but it works well in our project.
Part of the code was based on Martin Koch code. 

MIDI commands


Control Change = 33 to 37 decresases the delay_time from 1000 to 10ms;

Program Change Commands sends indivudual messages directly to cameras, one step for each message.

values between 01 and 09 send commands to camera 1
values between 10 and 19 send commands to camera 2
values between 20 and 29 send commands to camera 3

Program Change = 3,13,23 Focus Near    {0,0,0,1,0,1,0,0} {1,0,1,0,0,0,1,0}
Program Change = 4,14,24 Focus Far     {0,0,0,1,0,1,0,0} {1,1,1,0,0,0,1,0}
Program Change = 5,15,25 Auto Focus    {0,0,0,1,0,1,0,0} {1,1,0,0,0,0,1,0}
Program Change = 6,16,26 Auto Iris     {0,0,0,1,0,1,0,0} {1,1,1,1,0,1,0,1}
Program Change = 7,17,27 Iris -        {0,0,0,1,0,1,0,0} {1,1,0,0,1,0,1,0}
Program Change = 8,18,28 Iris +        {0,0,0,1,0,1,0,0} {1,0,1,0,1,0,1,0}

To send several steps (i.e. three IRIS+ steps) at once, you should send Contro Change MIDI messages
But you should select the camera first by using the fowling messages:

Program Change = 30 Select camera 1
Program Change = 31 Select camera 2
Program Change = 32 Select camera 3

Then... you can send messages to the selected camera and define the number of steps (maximum of 11 steps to the BMPC)

Control Change = 3 Focus Near
Control Change = 4 Focus Near
Control Change = 7 IRIS-
Control Change = 8 IRIS+

MIDI Example

Channel = 4
Control Change = 3
Value = 3 (the number of steps ... a value between 1 and 11)


2014, Grifu
v 1.2

more info: http://virtualmarionette.grifu.com or by mail virtual.marionette@grifu.com

*/





#define cmdPin_1 9 // writes tx to the Digital Pin 9 (camera 1)
#define lancPin_1 8  // reads rx from the Digital Pin 8

#define cmdPin_2 5 // writes tx the Digital Pin 5 (camera 2)
#define lancPin_2 4  // reads rx from Digital Pin 4

#define cmdPin_3 3 // writes tx the Digital Pin 3 (camera 3)
#define lancPin_3 2  // reads rx from Digital Pin 2
 
int cmdPin = 9; // writes tx the Digital Pin 9 (standard pins)
int lancPin= 8;  // reads rx from Digital Pin 8 

int cmdRepeatCount;
int bitDuration = 104; //Duration of one LANC bit in microseconds. 
 
int note = 0; // midi command to select camera and function 
int camera = 0; // define witch camera
int delay_time = 200; // delay time between messages

 
void setup() {
  MIDI.begin(4);
  // 115200 hairless MIDI
  Serial.begin(115200); // Althought LANC works with 9600 bps the Midi Hairless works with 115200


  pinMode(lancPin, INPUT); //listens to the LANC line
  pinMode(cmdPin, OUTPUT); //writes to the LANC line
  
  pinMode(lancPin_1, INPUT); //listens to the LANC line
  pinMode(cmdPin_1, OUTPUT); //writes to the LANC line
  pinMode(lancPin_2, INPUT); //listens to the LANC line
  pinMode(cmdPin_2, OUTPUT); //writes to the LANC line
  pinMode(lancPin_3, INPUT); //listens to the LANC line
  pinMode(cmdPin_3, OUTPUT); //writes to the LANC line

  digitalWrite(cmdPin_1, LOW); //set LANC line to +5V  
  digitalWrite(cmdPin_2, LOW); //set LANC line to +5V  
  digitalWrite(cmdPin_3, LOW); //set LANC line to +5V  
  
  digitalWrite(cmdPin, LOW); //set LANC line to +5V
  delay(5000); //Wait for camera to power up completly
  bitDuration = bitDuration - 8; //Writing to the digital port takes about 8 microseconds so only 96 microseconds are left till the end of each bit
 pinMode(13, OUTPUT);
 digitalWrite(13, HIGH);

                        
}
 
void loop() {
  

  
      if (MIDI.read())       // If we have received a message
    {
     int bin1[8] = {0,0,0,1,0,1,0,0}; 
     int bin2[8] = {1,0,1,0,1,0,1,0}; 
                        
        note = 0;
        byte type = MIDI.getType();
        switch (type) 
        {               
            case midi::ProgramChange:       // If it is a Program Change,  
                 
             note = MIDI.getData1();
             if (note < 9 || note == 30)  //camera 1 (30 = just select camera 1)
             {
              cmdPin = cmdPin_1; // writes tx
              lancPin = lancPin_1;  // reads rx
               camera = 1;

             } else if (note > 9 && note < 19 || note == 31) // camera 2 (31 = just select camera 2)
             {
              cmdPin = cmdPin_2; // writes tx
              lancPin = lancPin_2;  // reads rx
               camera = 2;

             } else if (note > 19 && note < 29 || note == 32)  // camera 3 (32 = just select camera 3) 
             {
              cmdPin = cmdPin_3; // writes tx
              lancPin = lancPin_3;  // reads rx
               camera = 3;

             } else // control the delay_time
             {
               if (note == 33) delay_time = 1000;
               if (note == 34) delay_time = 200;
               if (note == 35) delay_time = 100;
               if (note == 36) delay_time = 50;
               if (note == 37) delay_time = 10;
             }
             
             // Individual function change Step by Step
                  

                 if (note == 3 || note == 13 || note == 23 ) // Focus Near
                 {
                   MIDI.sendNoteOn(note,127,1); 
                        int bin1[8] = {0,0,0,1,0,1,0,0}; 
                        int bin2[8] = {1,0,1,0,0,0,1,0};  
                        LANC(bin1, bin2);
                        delay(delay_time);
                        MIDI.sendNoteOff(note,127,1);
                 }
                 if (note == 4 || note == 14 || note == 24 ) // Focus Far
                 {
                         MIDI.sendNoteOn(note,127,1); 
                        int bin1[8] = {0,0,0,1,0,1,0,0}; // Focus Far
                        int bin2[8] = {1,1,1,0,0,0,1,0};    
                        LANC(bin1, bin2);  
                        delay(delay_time);
                        MIDI.sendNoteOff(note,127,1);        
                 }
                 if (note == 5 || note == 15 || note == 25) // Auto Focus
                 {
                        MIDI.sendNoteOn(note,127,1); 
                        int bin1[8] = {0,0,0,1,0,1,0,0}; 
                        int bin2[8] = {1,1,0,0,0,0,1,0};  
                        LANC(bin1, bin2);
                        delay(delay_time);
                        MIDI.sendNoteOff(note,127,1);
                 }
                 if (note == 6 || note == 16 || note == 26) // Auto IRIS
                 {
                        MIDI.sendNoteOn(note,127,1); 
                        int bin1[8] = {0,0,0,1,0,1,0,0}; // Auto Iris 
                        int bin2[8] = {1,1,1,1,0,1,0,1};  
                        LANC(bin1, bin2);
                        delay(delay_time);
                        MIDI.sendNoteOff(note,127,1);
                 }
                 if (note == 7 || note == 17 || note == 27) //  IRIS- 11001010
                 {
                        MIDI.sendNoteOn(note,127,1); 
                        int bin1[8] = {0,0,0,1,0,1,0,0}; 
                        int bin2[8] = {1,1,0,0,1,0,1,0};  
                        LANC(bin1, bin2);
                        delay(delay_time);
                        MIDI.sendNoteOff(note,127,1);
                 }
                 if (note == 8 || note == 18 || note == 28) //  IRIS+ 10101010
                 {
                        MIDI.sendNoteOn(note,127,1); 
                        int bin1[8] = {0,0,0,1,0,1,0,0}; 
                        int bin2[8] = {1,0,1,0,1,0,1,0};  
                        LANC(bin1, bin2);
                        delay(delay_time);
                        MIDI.sendNoteOff(note,127,1);
                 }

                 
                 
                 // CONTROL for sequence change
                 
            case midi::ControlChange:       // If it is a Program Change,  
                  // program change number 2      
                 if (MIDI.getData1() == 4) // focus far
                 {
                   for (int x=0; x<MIDI.getData2();x++)
                   {
                        MIDI.sendNoteOn(4,127,1); 
                        note = 1; // note is playing
                        int bin1[8] = {0,0,0,1,0,1,0,0}; 
                        int bin2[8] = {1,1,1,0,0,0,1,0};  
                        LANC(bin1, bin2); 
                        delay(delay_time);
                        MIDI.sendNoteOff(4,0,1);
                   }
                 } else if (MIDI.getData1() == 3) // focus Near
                 {
                   
                   for (int x=0; x<MIDI.getData2();x++)
                   {
                        MIDI.sendNoteOn(3,127,1); 
                        note = 1; // note is playing
                        int bin1[8] = {0,0,0,1,0,1,0,0}; 
                        int bin2[8] = {1,0,1,0,0,0,1,0};  
                        LANC(bin1, bin2); 
                        delay(delay_time);
                        MIDI.sendNoteOff(3,0,1);
                   }
                 } else if (MIDI.getData1() == 7) // IRIS-
                 {
                   
                   for (int x=0; x<MIDI.getData2();x++)
                   {
                        MIDI.sendNoteOn(7,127,1); 
                        note = 1; // note is playing
                        int bin1[8] = {0,0,0,1,0,1,0,0}; 
                        int bin2[8] = {1,1,0,0,1,0,1,0};  
                        LANC(bin1, bin2); 
                        delay(delay_time);
                        MIDI.sendNoteOff(7,0,1);
                   }
                 } else if (MIDI.getData1() == 8) // IRIS+
                 {
                   
                   for (int x=0; x<MIDI.getData2();x++)
                   {
                        MIDI.sendNoteOn(8,127,1); 
                        note = 1; // note is playing
                        int bin1[8] = {0,0,0,1,0,1,0,0}; 
                        int bin2[8] = {1,0,1,0,1,0,1,0};  
                        LANC(bin1, bin2); 
                        delay(delay_time);
                        MIDI.sendNoteOff(8,0,1);
                   }
                 }  
                 
                 
        }
        
            
    }

    
 
    
    
}

// function to write binnary to LANC
 void Func(int bin[8])
 {
 //   int bin[8] = {0,0,0,1,1,0,0,0};
    for (int x=0;x <8; x++)
    {

     if (bin[x] == 0) digitalWrite(cmdPin, LOW);
     if (bin[x] == 1) digitalWrite(cmdPin, HIGH); 
     delayMicroseconds(bitDuration);
     
    }
 }


// function to control LANC

void LANC(int cmd1[8], int cmd2[8]) 
{


  cmdRepeatCount = 0;

  while (cmdRepeatCount < 5) 
  {  
  //repeat 5 times to make sure the camera accepts the command

    while (pulseIn(lancPin, HIGH) < 5000) 
    {   
     //"pulseIn, HIGH" catches any 0V TO +5V TRANSITION and waits until the LANC line goes back to 0V  //Loop till pulse duration is >5ms 
    } 

  //LOW after long pause means the START bit of Byte 0 is here
  delayMicroseconds(bitDuration);  //wait START bit duration
  
  Func(cmd1); //Function to send the first byte
  //Byte 0 is written now put LANC line back to +5V
  digitalWrite(cmdPin, LOW);
  delayMicroseconds(10); //make sure to be in the stop bit before byte 1

  while (digitalRead(lancPin)) 
  { 
    //Loop as long as the LANC line is +5V during the stop bit
  }

  //0V after the previous stop bit means the START bit of Byte 1 is here
  delayMicroseconds(bitDuration);  //wait START bit duration

  Func(cmd2); // Function to send the second byte

  //Byte 1 is written now put LANC line back to +5V
  digitalWrite(cmdPin, LOW); 

  cmdRepeatCount++;  //increase repeat count by 1

  /*Control bytes 0 and 1 are written, now don’t care what happens in Bytes 2 to 7
  and just wait for the next start bit after a long pause to send the first two command bytes again.*/


  }//While cmdRepeatCount < 5
}
