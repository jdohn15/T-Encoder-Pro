/*
 * @Description: None
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-07-03 17:37:09
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2024-01-09 18:46:23
 * @License: GPL 3.0
 */
#include <FastLED.h>

#define NUM_LEDS 1
#define DATA_PIN 18
CRGB leds[NUM_LEDS];

void setup()
{
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is typical
    FastLED.setBrightness(50);

    leds[0] = CRGB::Red;
    FastLED.show();
    delay(1000);

    leds[0] = CRGB::Green;
    FastLED.show();
    delay(1000);

    leds[0] = CRGB::Blue;
    FastLED.show();
    delay(1000);
}

void loop()
{
    for (int i = 0; i < 5; i++)
    {
        // Turn the LED on, then pause
        leds[i] = CRGB::Red;
        FastLED.show();
        delay(500);
        // Now turn the LED off, then pause
        leds[i] = CRGB::Black;
        FastLED.show();
        delay(500);
    }
}
