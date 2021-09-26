/*
    Project     'MacroBoard' Mini Macro Keyboard
    @author     Moritz Lerch
    @link       github.com/MoritzLerch
    @license    MIT - Copyright (c) 2021 Moritz Lerch

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.

*/

// ---------------------------------
// Key definitions
#define BUTTON_KEY1 KEY_F13
#define BUTTON_KEY2 KEY_F14
#define BUTTON_KEY3 KEY_F15
#define BUTTON_KEY4 KEY_F16
#define BUTTON_KEY5 KEY_F17
#define BUTTON_KEY6 KEY_F18
#define BUTTON_KEY7 KEY_F19
#define BUTTON_KEY8 KEY_F20

// Pin definitions
#define BUTTON_PIN1 1
#define BUTTON_PIN2 2
#define BUTTON_PIN3 3
#define BUTTON_PIN4 4
#define BUTTON_PIN5 5
#define BUTTON_PIN6 6
#define BUTTON_PIN7 7
#define BUTTON_PIN8 8
#define debugLED LED_BUILTIN
// ---------------------------------

#include "Arduino.h"
#include "Keyboard.h"
#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 1
#define DATA_PIN 9

Adafruit_NeoPixel frontLED(NUMPIXELS, DATA_PIN, NEO_RGB + NEO_KHZ800);

int globalBrightness = 80;
int globalColorR = 255;
int globalColorG = 0;
int globalColorB = 0;

void setPixelColorBrightness(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint16_t brightness)
{
  frontLED.setPixelColor(n, (brightness * r / 255), (brightness * g / 255), (brightness * b / 255));
}

void colorFade(uint8_t r, uint8_t g, uint8_t b, uint8_t wait, uint8_t pixelDelay) {
  for(uint16_t i = 0; i < frontLED.numPixels(); i++) {
      uint8_t curr_r, curr_g, curr_b;
      uint32_t curr_col = frontLED.getPixelColor(i); // get the current colour
      curr_b = curr_col & 0xFF; curr_g = (curr_col >> 8) & 0xFF; curr_r = (curr_col >> 16) & 0xFF;  // separate into RGB components

      while ((curr_r != r) || (curr_g != g) || (curr_b != b)){  // while the curr color is not yet the target color
        if (curr_r < r) curr_r++; else if (curr_r > r) curr_r--;  // increment or decrement the old color values
        if (curr_g < g) curr_g++; else if (curr_g > g) curr_g--;
        if (curr_b < b) curr_b++; else if (curr_b > b) curr_b--;
        frontLED.setPixelColor(i, curr_r, curr_g, curr_b);  // set the color
        frontLED.show();
        delay(wait);  // add a delay if its too fast
      }
      delay(pixelDelay);
  }
}

void colorFade(uint8_t r, uint8_t g, uint8_t b, uint16_t brightness, uint8_t wait, uint8_t pixelDelay) {
  for(uint16_t i = 0; i < frontLED.numPixels(); i++) {
      uint8_t curr_r, curr_g, curr_b;
      uint32_t curr_col = frontLED.getPixelColor(i); // get the current colour
      curr_b = curr_col & 0xFF; curr_g = (curr_col >> 8) & 0xFF; curr_r = (curr_col >> 16) & 0xFF;  // separate into RGB components

      r = brightness * r / 255;
      g = brightness * g / 255;
      b = brightness * b / 255;

      while ((curr_r != r) || (curr_g != g) || (curr_b != b)){  // while the curr color is not yet the target color
        if (curr_r < r) curr_r++; else if (curr_r > r) curr_r--;  // increment or decrement the old color values
        if (curr_g < g) curr_g++; else if (curr_g > g) curr_g--;
        if (curr_b < b) curr_b++; else if (curr_b > b) curr_b--;
        frontLED.setPixelColor(i, curr_r, curr_g, curr_b);  // set the color
        frontLED.show();
        delay(wait);  // add a delay if its too fast
      }
      delay(pixelDelay);
  }
}

void startupAnimation(int brightness, int fadeDuration, int rainbowDelay, int cycles)
{
  int fadeDelay = fadeDuration / brightness;

  for (int i = 0; i <= 255; i++)
  {
    frontLED.setPixelColor(0, frontLED.Color(i, 0, 0));
    frontLED.show();
    delay(fadeDelay);
  }

  for (int i = 0; i < cycles; i++)
  {
    for (long firstPixelHue = 0; firstPixelHue < 65536; firstPixelHue += 256)
    {
      for (int i = 0; i < frontLED.numPixels(); i++)
      {
        int pixelHue = firstPixelHue + (i * 65536L / frontLED.numPixels());
        frontLED.setPixelColor(i, frontLED.gamma32(frontLED.ColorHSV(pixelHue)));
      }
      frontLED.show();
      delay(rainbowDelay);
    }
  }
}

// Button helper class for handling press/release and debouncing
class button
{
public:
  const char key;
  const uint8_t pin;

  button(uint8_t k, uint8_t p) : key(k), pin(p) {}

  void press(boolean state)
  {
    if (state == pressed || (millis() - lastPressed <= debounceTime))
    {
      return; // Nothing to see here, folks
    }

    lastPressed = millis();

    state ? Keyboard.press(key) : Keyboard.release(key);

    if (state == 1)
    {
      visualFeedback();
    }

    pressed = state;
  }

  void visualFeedback()
  {
    setPixelColorBrightness(0, globalColorR, globalColorG, globalColorB, globalBrightness + 150);
    // frontLED.setPixelColor(0, frontLED.Color(globalBrightness + 150, 0, 0));
    frontLED.show();
    digitalWrite(debugLED, LOW);
    delay(100);
    setPixelColorBrightness(0, globalColorR, globalColorG, globalColorB, globalBrightness);
    // frontLED.setPixelColor(0, frontLED.Color(globalBrightness, 0, 0));
    frontLED.show();
    digitalWrite(debugLED, HIGH);
  }

  void update()
  {
    press(!digitalRead(pin));
  }

private:
  const unsigned long debounceTime = 50;
  unsigned long lastPressed = 0;
  boolean pressed = 0;
};

// Button objects, organized in array
button buttons[] = {
    {BUTTON_KEY1, BUTTON_PIN1},
    {BUTTON_KEY2, BUTTON_PIN2},
    {BUTTON_KEY3, BUTTON_PIN3},
    {BUTTON_KEY4, BUTTON_PIN4},
    {BUTTON_KEY5, BUTTON_PIN5},
    {BUTTON_KEY6, BUTTON_PIN6},
    {BUTTON_KEY7, BUTTON_PIN7},
    {BUTTON_KEY8, BUTTON_PIN8},
};

const uint8_t NumButtons = sizeof(buttons) / sizeof(button);

void setup()
{
  frontLED.begin();
  frontLED.setBrightness(globalBrightness);

  // Set LEDs Off. Active low.
  pinMode(debugLED, OUTPUT);
  digitalWrite(debugLED, HIGH);

  for (int i = 0; i < NumButtons; i++)
  {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }

  startupAnimation(globalBrightness, 1000, 15, 1);
  colorFade(globalColorR, globalColorG, globalColorB, globalBrightness, 5, 0);
}

void loop()
{
  for (int i = 0; i < NumButtons; i++)
  {
    buttons[i].update();
  }
}
