/*
This is a wireless joysetick receiver using an NRF20L01 module and an atmega 32u4 usb connection.
This program can receive all the axes and stuff that you can send, however you don't need to have all that stuff on your transmitter.
You shouldn't have to etch a pcb for this, it's just the arduino and the NRF.
The built in LED will blink whenever it receives a signal. It should be lit fairly constantly when the transmitter is on. 
If it's blinking, your wireless signal is sporadic.

Latency is around 7-8ms.

Radio    Arduino
CE    -> 4
CSN   -> 3 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI)
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND

*/

//comment this out when done testing. For some reason having the serial port open when not reading it makes it not work right.
//#define DEBUG

#include <SPI.h>
#include <NRFLite.h>

const static uint8_t RADIO_ID = 0;       // Our radio's id.  The transmitter will send to this id.
const static uint8_t PIN_RADIO_CE = 4;
const static uint8_t PIN_RADIO_CSN = 3;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    int FromRadioId;
    int xaxis;
    int yaxis;
    int zaxis;
    int rxaxis;
    int ryaxis;
    int rzaxis;
    int throttle;
    int rudder;
    int buttons;
};

NRFLite _radio;
RadioPacket _radioData;

#include "Joystick.h"

// Create Joystick
Joystick_ Joystick;

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  #ifdef DEBUG
  Serial.begin(115200);
  #endif



    // By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
    // Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
    // You can run the 'ChannelScanner' example to help select the best channel for your environment.
    // You can assign a different bitrate and channel as shown below.
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100) // THE DEFAULT
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        #ifdef DEBUG
        Serial.println("Cannot communicate with radio");
        #endif
        while (1) {
          digitalWrite(LED_BUILTIN,HIGH);
          delay(500);
          digitalWrite(LED_BUILTIN,LOW);
          delay(500);
          #ifdef DEBUG
          Serial.println("Cannot communicate with radio");
          #endif
        }
    }
    #ifdef DEBUG
    Serial.println("connected");
    #endif
    
Joystick.setXAxisRange(0, 1024);
Joystick.setYAxisRange(0, 1024);
Joystick.setZAxisRange(0, 1024);
Joystick.setRxAxisRange(0, 1024);
Joystick.setRyAxisRange(0, 1024);
Joystick.setRzAxisRange(0, 1024);
Joystick.setThrottleRange(0, 1024);
Joystick.setRudderRange(0, 1024);
Joystick.begin(0);
}

void loop() {
  

  //update the joystick when you get a radio packet.
  while (_radio.hasData())
    {
        #ifdef DEBUG
        Serial.println("received");
        #endif
        digitalWrite(LED_BUILTIN,HIGH);
        _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.

        String msg = "Radio ";
        // _radioData.FromRadioId;
        Joystick.setXAxis(_radioData.xaxis);
        Joystick.setYAxis(_radioData.yaxis);
        Joystick.setZAxis(_radioData.zaxis);
        Joystick.setRxAxis(_radioData.rxaxis);
        Joystick.setRyAxis(_radioData.ryaxis);
        Joystick.setRzAxis(_radioData.rzaxis);
        Joystick.setThrottle(_radioData.throttle);
        Joystick.setRudder(_radioData.rudder);
        int binbutton = _radioData.buttons;

        //binbutton acts as an array of bools indicating status of buttons. this reads them and sets the joy buttons accordingly.
        for (int button = 0; button < 16; button++) {
          bool butt = bitRead(binbutton,button);
          if (butt == 1) {
            Joystick.pressButton(button);
          }
          else {
            Joystick.releaseButton(button);
          }
          
        }

      //send updates to computer
      Joystick.sendState();  
      digitalWrite(LED_BUILTIN,LOW);

}
}
