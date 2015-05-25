# LEDGradientAnimations

Creating LED animations on a matrix using a gradient at an angle moving it left and right.

Using a Arduino Uno, 25xWS2811LEDs, and the FastLED library

GradientLEFT() and GradientRight() animations: http://youtu.be/gn0vJ-lxXrQ

UPDATE:
LEDGradientGogglesRefArray - No longer use a 2d array to map colors as I was only using 32 points out of 256. Instead I have one array the holds the color information for each pixel (as if they were still in the 2d matrix) and determine the color for each shift L or R.
M
The LED goggles were made mostly from following the guide on Adafruit https://learn.adafruit.com/kaleidoscope-eyes-neopixel-led-goggles-trinket-gemma/overview
Some changes I made:
- Used the TinyLily instead of the Trinket/Gemma https://tiny-circuits.com/tiny-lily-mini-processor.html
- Gradient animation moves left or right depending on directional movement. (using a digital compas LSM303 https://learn.adafruit.com/flora-sensors/accelerometer-slash-compass-sensor-lsm303) 

I moved the LSM303 outside the goggle casing and I am trying to keep it flat to get accurate and consistent readings. 
Youtube Demo: http://youtu.be/kMAGdWFNmQE