[![Build Status](https://travis-ci.org/pezinek/hint_box.svg?branch=master)](https://travis-ci.org/pezinek/hint_box)

# Hint Box

Hint Box is meant as a game for kids where they look for hidden iButtons and get additional instructions/hints for each new iButton they find.
So far the instructions are hard-coded and every new game requires rewrite of the messages and button mappings in the code and recompiling. 

## Hardware used

- Arduino Uno
- LCD Keypad Shield
- [iButton Reader](https://www.aliexpress.com/item/TM-probe-DS9092-Zinc-Alloy-probe-iButton-probe-reader-with-LED/32702820667.html)
- Resistors 4.7 kΩ and 180 Ω
- optionally some batteries and switch to make it mobile.

### Wiring

- iButton Reader DATA (Green) -> Arduino pin 2
- iButton Reader DATA (Green) -> R 4.7 kΩ -> Arduino 5V
- iButton GND (Red) -> Arduino GND
- iButton Led + (Black) -> R 180 Ω -> Arduino pin 3
- iButton Led - (White) -> Arduino GND

## Software

If you plan to re-use this game go to ```src/main.cpp``` and modify the messages in ```msgs[]``` and the iButton mapping in ```address_map[]``` and recompile.

## FixMe

- Due to the memmory limitation of the Arduino UNO the number of registered hints is limitted to approx. 10 (based on their lenght)
