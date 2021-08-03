/*
This is for a pro mini and an NRF24L01 transmitter module.
The arduino reads the pins in bpins, and analog inputs A0-A5 and sends them to the receiver.
bpins are pullups, pull them to ground to turn on the button.

It uses the 2 pin wiring for the NRF as described in the NRFLite documentation.

Disable debug when doing final implementation, it increases latency significantly.

Please read through 'NRFLite.h' for a description of all the methods available in the library.

Connections
* Arduino 11  > MOMI of 2-pin circuit
* Arduino 12 > SCK of 2-pin circuit

*/

//#define DEBUG


#include <SPI.h>
#include <NRFLite.h>

const static uint8_t RADIO_ID = 2;             // Our radio's id.
const static uint8_t DESTINATION_RADIO_ID = 0; // Id of the radio we will transmit to.
const static uint8_t PIN_RADIO_MOMI = 11;
const static uint8_t PIN_RADIO_SCK = 12;


static int bpins[9] = {2,3,4,5,6,7,8,9,10};
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

int button = 0;


void setup()
{
    #ifdef DEBUG
    Serial.begin(115200);
    #endif
    for (int i = 0; i < 9; i++) {
      pinMode(bpins[i],INPUT_PULLUP);
    }


    
    // By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
    // Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
    // You can run the 'ChannelScanner' example to help select the best channel for your environment.
    // You can assign a different bitrate and channel as shown below.
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100) // THE DEFAULT
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)
    
    if (!_radio.initTwoPin(RADIO_ID, PIN_RADIO_MOMI, PIN_RADIO_SCK))
    {
        #ifdef DEBUG
        Serial.println("Cannot communicate with radio");
        #endif
        while (1); // Wait here forever.
    }
    
    _radioData.FromRadioId = RADIO_ID;
}

void loop()
{
    
    //this reads digital inputes and maps them to buttons.
    for (int i = 0; i < 9; i++) {
      bitWrite(button,i,!digitalRead(bpins[i]));
    }
    _radioData.buttons = button;

    //this reads analog inputs and maps them to axes, use a constant for unmapped axes like 512
    _radioData.xaxis = analogRead(A0);
    _radioData.yaxis = analogRead(A1);
    _radioData.zaxis = analogRead(A2);
    _radioData.rxaxis = analogRead(A3);
    _radioData.ryaxis = analogRead(A4);
    _radioData.rzaxis = analogRead(A5);
    _radioData.throttle = 512;
    _radioData.rudder = 512;

    
    // By default, 'send' transmits data and waits for an acknowledgement.  If no acknowledgement is received,
    // it will try again up to 16 times.  This retry logic is built into the radio hardware itself, so it is very fast.
    // You can also perform a NO_ACK send that does not request an acknowledgement.  In this situation, the data packet
    // will only be transmitted a single time and there is no verification of delivery.  So NO_ACK sends are suited for
    // situations where performance is more important than reliability.
    //   _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::REQUIRE_ACK) // THE DEFAULT
    //   _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::NO_ACK)
    
    if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))) // Note how '&' must be placed in front of the variable name.
    {
        #ifdef DEBUG
        Serial.println("...Success");
        #endif
    }
    else
    {
        #ifdef DEBUG
        Serial.println("...Failed");
        #endif
        
    }
    //delay(5);
    
}
