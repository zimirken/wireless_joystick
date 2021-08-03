# wireless_joystick
Wireless Joystick boards using NRF24L01 modules and an Arduino Micro for PC connection

I wanted to maky my own button box for Star Citizen. (and later, a small steering wheel for Farming Simulator)
I like wireless because I don't have a dedicated "gaming station". I play on the couch where animals and toddlers roam.
I don't want a long cable that can be tripped on.

I never noticed any input lag while using it, but eventually curiosity got the best of me. I set up a feedback loop where the receiver had an output pin that it would set to the state of one of the buttons after the receiver was done receiving and updating the usb. This was connected to the input on the transmitter (which is inverted - pull low for on).
This set up a feedback loop where the TX would see the input HIGH, and send the button off state to the RX, which would set its output LOW, which would pull the TX input pin LOW. Then the next time the TX transmitted, it would see the input was LOW, and send the button state as ON, which the RX would see and turn the output HIGH, and the cycle would repeat. 

Doing this I found that the total latency was an extremely steady 7-8ms. This is perfect for all but the most hardcore gamers, and is a lower latency than most current gen console wireless controllers.

However, when I tested multiple transmitters into one receiver, the response time bounced around from the 7ms minimum up to 20ms+. So for time critical inputs, you want one transmitter and one receiver per channel.

This is waaaay better than I was expecting. Which means this is a perfectly viable method for wireless joystick control.

So I'm posting my code here. It's pretty straightforward so you shouldn't have any trouble. Make sure you check to make sure the code matches your wiring for the NRF module. Some of these programs are set for two pin wiring and others for four pin.

Link to thingiverse files for the mini steering wheel here: https://www.thingiverse.com/thing:4923941
