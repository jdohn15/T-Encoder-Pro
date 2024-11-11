/*
 * @Description: None
 * @version: V1.0.0
 * @Author: None
 * @Date: 2023-08-29 13:34:55
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-09-18 18:03:50
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "pin_config.h"

uint8_t i = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(LCD_MOSI, OUTPUT);
    pinMode(LCD_SCLK, OUTPUT);
    pinMode(LCD_CS, OUTPUT);
    pinMode(LCD_DC, OUTPUT);
    pinMode(LCD_RST, OUTPUT);
    pinMode(LCD_BL, OUTPUT);

    digitalWrite(LCD_MOSI, i);
    digitalWrite(LCD_SCLK, i);
    digitalWrite(LCD_CS, i);
    digitalWrite(LCD_DC, i);
    digitalWrite(LCD_RST, i);
    digitalWrite(LCD_BL, i);

    // pinMode(LCD_SCLK, OUTPUT);
    // pinMode(LCD_CS, OUTPUT);
    // pinMode(LCD_RST, OUTPUT);
    // pinMode(LCD_SDIO0, OUTPUT);
    // pinMode(LCD_SDIO1, OUTPUT);
    // pinMode(LCD_SDIO2, OUTPUT);
    // pinMode(LCD_SDIO3, OUTPUT);

    // digitalWrite(LCD_SCLK, i);
    // digitalWrite(LCD_CS, i);
    // digitalWrite(LCD_RST, i);
    // digitalWrite(LCD_SDIO0, i);
    // digitalWrite(LCD_SDIO1, i);
    // digitalWrite(LCD_SDIO2, i);
    // digitalWrite(LCD_SDIO3, i);
}

void loop()
{
    // digitalWrite(LCD_MOSI, i);
    digitalWrite(LCD_SCLK, i);
    // digitalWrite(LCD_CS, i);
    // digitalWrite(LCD_DC, i);
    // digitalWrite(LCD_RST, i);
    // digitalWrite(LCD_BL, i);

    // digitalWrite(LCD_SCLK, i);
    // digitalWrite(LCD_CS, i);
    // digitalWrite(LCD_RST, i);
    // digitalWrite(LCD_SDIO0, i);
    // digitalWrite(LCD_SDIO1, i);
    // digitalWrite(LCD_SDIO2, i);
    // digitalWrite(LCD_SDIO3, i);
    i = !i;
    delay(3000);
}