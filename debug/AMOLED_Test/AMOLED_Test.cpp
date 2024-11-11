/*
 * @Description: None
 * @version: V1.0.0
 * @Author: None
 * @Date: 2023-09-09 10:18:45
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-09-09 17:52:31
 * @License: GPL 3.0
 */
#include "rm67162.h"
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

void setup()
{
    Serial.begin(115200);
    rm67162_init(); // amoled lcd initialization
                    //   lcd_display_invert_on();

    //   lcd_address_set(0, 0, 466, 466);
    // for (int i = 0; i < 466; i++)
    // {
    //     for (int j = 0; j < 466; j++)
    //     {
    //         lcd_PushColors(color,466);
    //     }
    // }

    lcd_fill(0, 0, 233, 466, TFT_GREEN);

    // sprite.createSprite(466, 466);
    //   sprite.setSwapBytes(1);
}

void draw()
{
    // sprite.fillSprite(TFT_BLACK);
    // sprite.drawString("Hello World", 20, 20, 4);
    // sprite.fillRect(10, 100, 60, 60, TFT_RED);
    // sprite.fillRect(80, 100, 60, 60, TFT_GREEN);
    // sprite.fillRect(150, 100, 60, 60, TFT_BLUE);

    // lcd_PushColors(0, 0, 233, 466, (uint16_t *)sprite.getPointer());
}

void loop()
{
    draw();
}