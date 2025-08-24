## u8x8 Over Serial

Let's you control a u8x8 display connected to a microcontroller using commands over serial.

Built in platform.io with the arduino framework, use this to compile the code for your specific microcontroller platform.

### Commands
- CLEAR
Clears the screen
- DRAW_CHAR X Y C
Draws character C at pos X,Y
- DRAW_CHAR_12 X Y C
Draws 1x2 character C at pos X,Y
- DRAW_CHAR_22 X Y C
Draws 2x2 character C at pos X,Y
- DRAW_STRING X Y S
Draws string S at pos X,Y
- DRAW_STRING_12 X Y S
Draws 1x2 string S at pos X,Y
- DRAW_STRING_22 X Y S
Draws 2x2 string S at pos X,Y

### Serial
115200 8N1

Commands are sent as follows
```
COMMAND$PARAM1$PARAM2$PARAM3$CHECKSUM\n
```
The delimiter '$' can be changed at the top of the main.cpp.

Checksum is calculated using the sum of the characters ascii value modulo 256 (excluding the delimiters, eol, and checksum itself) and sent as one character.
