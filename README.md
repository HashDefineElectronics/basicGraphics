# Hash Define Electronics basic graphics library

### version 0.0.0
### Author Ronald Sousa - [@HashDefineElec](https://twitter.com/#!/HashDefineElec) - [hashdefineelectronics.com](https://www.hashdefineelectronics.com)

This library was created to be a simple go to graphics library for getting basic graphics onto a small display. Its intended to provide font, primitive shapes and bitmap drawing on to the screen.

The idea is so if you have an embedded project (MCU/CPU) that require graphical screen such as an oled or TFT to display text, icons or basic shapes then this library can help. Its be design to be hardware independent by moving out any hardware specific code to its on module that you can write driver too.

### Note
- First this library is still underdevelopment and its not production ready.

This library's font render uses [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) fontconvert as well as some of their code to render fonts. That been said, we are planning at some point take a copy of fontconvert and updated to suite this projects needs to ensure that generated fonts match this library format.

### credit
I want to mention that the font rendering code was taken out from

## What can it do?
To be honest, the library currently only supports display drivers and font rendering. Might be usefull to you if you only need to dixplay text onto a screen.

-

## Todo
Considering that this graphics library is still in its early development cycle there still plenty to do.

- Render line
- [Bresenham's algorithm](https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm)
- Primitieve shapes
- box/rectangle
- circle
- triangle
- Render bitmap
- Colour
- integration Examples
- Unit and integration test
- copy and update [fontconvert](https://github.com/adafruit/Adafruit-GFX-Library/tree/master/fontconvert)
- Create more example fonts
- font draw scale