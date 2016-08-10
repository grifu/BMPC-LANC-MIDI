------------------------------------------------------------------------------------------
BMPC-LANC-MIDI v1.2 (2014 www.grifu.com)

LANC Remote Control IRIS/FOCOUS for Blackmagic Pocket Camera (BMPC)

LANC to MIDI to be used with BMPC.
Download Arduino MIDI library from http://playground.arduino.cc/Main/MIDILibrary

Download Hairless-midi from http://projectgus.github.io/hairless-midiserial/

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

LANC commands are defined in Hexadecimal converted to binary translated to pulses.

0x28, 0x45 = Focus Near  (reverse binary {0,0,0,1,0,1,0,0} {1,0,1,0,0,0,1,0})
0x28, 0x47 = Focus Far  (reverse binary {0,0,0,1,0,1,0,0} {1,1,1,0,0,0,1,0})
0x28, 0x53 = IRIS-  (reverse binary {0,0,0,1,0,1,0,0} {1,1,0,0,1,0,1,0})
0x28, 0x55 = IRIS+  (reverse binary {0,0,0,1,0,1,0,0} {1,0,1,0,1,0,1,0})
0x28, 0x43 = Auto Focus  (reverse binary {0,0,0,1,0,1,0,0} {1,1,0,0,0,0,1,0})
0x28, 0xAF = Auto IRIS  (reverse binary {0,0,0,1,0,1,0,0} {1,1,1,1,0,1,0,1})
0x18, 0x33 = REC (reverse binary {0,0,0,1,1,0,0,0} {1,1,0,0,1,1,0,0})


This code is very draft and could be optimized, but it works well in our project “Peregrino”
Part of the code was based on Martin Koch code.

MIDI commands
Control Change = 33 to 37 decresases the delay_time from 1000 to 10ms;

Program Change Commands sends indivudual messages directly to cameras, one step for each message.

values between 01 and 09 send commands to camera 1
values between 10 and 19 send commands to camera 2
values between 20 and 29 send commands to camera 3

Program Change = 3,13,23 Focus Near {0,0,0,1,0,1,0,0} {1,0,1,0,0,0,1,0}
Program Change = 4,14,24 Focus Far {0,0,0,1,0,1,0,0} {1,1,1,0,0,0,1,0}
Program Change = 5,15,25 Auto Focus {0,0,0,1,0,1,0,0} {1,1,0,0,0,0,1,0}
Program Change = 6,16,26 Auto Iris {0,0,0,1,0,1,0,0} {1,1,1,1,0,1,0,1}
Program Change = 7,17,27 Iris - {0,0,0,1,0,1,0,0} {1,1,0,0,1,0,1,0}
Program Change = 8,18,28 Iris + {0,0,0,1,0,1,0,0} {1,0,1,0,1,0,1,0}

To send several steps (i.e. three IRIS+ steps) at once, you should send Contro Change MIDI messages
But you should select the camera first by using the fowling messages:

Control Change = 30 Select camera 1
Control Change = 31 Select camera 2
Control Change = 32 Select camera 3

Then... you can send messages to the selected camera and define the number of steps (maximum of 11 steps to the BMPC)

Control Change = 3 Focus Near
Control Change = 4 Focus Near
Control Change = 7 IRIS-
Control Change = 8 IRIS+

MIDI Example

Channel = 4
Control Change = 3
Value = 3 (the number of steps ... a value between 1 and 11)
----------------------------------------------------------------------------------------------------
Grifu

http://www.virtualmarionette.grifu.com