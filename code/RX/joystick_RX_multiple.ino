/*
This is a wireless joysetick receiver using an NRF20L01 module and an arduino micro usb connection.

This program was built to test receiving from multiple transmitters. It reads the Y, Z, and RX axes from a transmitter using ID 1.
It reads all the buttons and the other axes from a transmitter with ID 2.

Commented out is code to allow you to hook pin 8 on the receiver to button 0 on the transmitter (pin 2 in my code) to create a feedback loop.
This loop allows you to test the latency of the system. With one transmitter it's a consistent 8ms. However with two transmitters it gets jumpy.


The built in LED will blink whenever it receives a signal. It should be lit constantly when the transmitter is on. 
If it's blinking, your reception is spotty.




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
#include <digitalWriteFast.h>

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
  //pinMode(8,OUTPUT);
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
        digitalWriteFast(LED_BUILTIN,HIGH);
        _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.

        String msg = "Radio ";
        if (_radioData.FromRadioId == 1) { //steering wheel
          
          Joystick.setYAxis(_radioData.yaxis);
          Joystick.setZAxis(_radioData.zaxis);
          Joystick.setRxAxis(_radioData.rxaxis);
        }
        if (_radioData.FromRadioId == 2) { //button box
          Joystick.setXAxis(_radioData.xaxis);
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
          //digitalWriteFast(8,bitRead(binbutton,0)); 
        }

      //send updates to computer
      Joystick.sendState(); 
      
      digitalWriteFast(LED_BUILTIN,LOW);

}
}
