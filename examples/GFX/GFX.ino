/*
 * @Description:GFX test program
 * @Author: LILYGO_L
 * @Date: 2023-09-06 10:58:19
 * @LastEditTime: 2024-11-11 14:00:46
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"

// DXQ120MYB2416A
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    SCREEN_CS /* CS */, SCREEN_SCLK /* SCK */, SCREEN_SDIO0 /* SDIO0 */,
    SCREEN_SDIO1 /* SDIO1 */, SCREEN_SDIO2 /* SDIO2 */, SCREEN_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_SH8601(bus, SCREEN_RST /* RST */, 0 /* rotation */,
                                      false /* IPS */, SCREEN_WIDTH, SCREEN_HEIGHT);

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(SCREEN_EN, OUTPUT);
    digitalWrite(SCREEN_EN, HIGH);

    gfx->begin();
    gfx->fillScreen(PALERED);

    for (uint8_t i = 0; i < 255; i++)
    {
        gfx->Display_Brightness(i);
        delay(3);
    }
    for (uint8_t i = 0; i < 4; i++)
    {
        gfx->SetContrast(i);
        delay(2000);
    }
    gfx->SetContrast(SH8601_ContrastOff);

    gfx->setCursor(20, 250);
    gfx->setTextColor(YELLOW);
    gfx->println("Ciallo1~(L *##*L)^**");
}

void loop()
{
    // gfx->fillScreen(PALERED);
    delay(1000);
}
