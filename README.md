# 2020-mask
talking LED mask for BSides 2020 presenters

derived from https://github.com/TylerGlaiel/voicemask/blob/master/mask.ino, archive available: https://web.archive.org/web/20200618173024/https://github.com/TylerGlaiel/voicemask/blob/master/mask.ino

## Parts:
- [$3.00 Arduino Pro Micro](https://www.aliexpress.com/item/32846843498.html)
- [$0.88 MAX4466 microphone module](https://www.aliexpress.com/item/32732472453.html)
- [$5.00 flexible 8x8 WS2812 matrix](https://www.aliexpress.com/item/1000007090230.html)
- $0.05 SMT switch
- Cotton face mask
- USB power pack....

## Wiring:
No additional wire needed, the LED panels probably already have it. Strip the wires, thread them through the arduino pin holes, and solder:
- LED Matrix 5v to Arduino RAW
- LED Matrix GND to Arduino GND
- LED Matrix DIN to Arduino A3 (digital 21)

Add the microphone. No wire needed, can solder directly to the arduino. We'll use I/O pins for software-controllable power and ground:
- Microphone VCC to Arduino 14
- Microphone GND to Arduino 15
- Microphone OUT to Arduino A0

Add a switch. Again, solder right to the arduino spanning a few pins (may vary based on how big your switch is):
- Switch end to Arduino 9
- Switch end to Arduion 5

## Software:
- clone this repo into your Arduino directory
  - on linux this is usually ~/Arduino
  - this should create a file ~/Arduino/2020-mask/2020-mask.ino
- go to 'Tools'->'Manage Libraries'
- search for and install:
  - Adafruit NeoMatrix
  - Adafruit NeoPixel
  - Adafruit GFX Library

## Programming:
- Choose 'Tools'->'Board'->'Arduino Leonardo'
- Choose the correct serial port (probably autoselected)
- Click "upload"
