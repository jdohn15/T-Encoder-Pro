/*
 * @Description: 播放图片
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-07-25 13:45:02
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-11-22 16:08:52
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "pin_config.h"
// #include "material_24Bit_480x480.h"
// #include "material_16Bit_480x480.h"
#include "material_24Bit_128x128.h"

#if defined NFP099B_01A
// NFP099B-01A
Arduino_DataBus *bus = new Arduino_SWSPI(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */);

Arduino_GFX *gfx = new Arduino_GC9107(
    bus, LCD_RST /* RST */, 0 /* rotation */, true /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);
#endif

#if defined DO0143FAT01
// DO0143FAT01
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */, LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST /* RST */, 0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);
#endif

#if defined DXQ120MYB2416A
// DXQ120MYB2416A
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */,
    LCD_SDIO1 /* SDIO1 */, LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST /* RST */, 0 /* rotation */,
                                      false /* IPS */, LCD_WIDTH, LCD_HEIGHT);
#endif

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Ciallo");

#if defined DXQ120MYB2416A
    // DXQ120MYB2416A
    pinMode(LCD_VCI_EN, OUTPUT);
    digitalWrite(LCD_VCI_EN, HIGH);
#endif

#if defined NFP099B_01A
    // NFP099B-01A
    ledcSetup(0, 20000, 8);
    ledcAttachPin(LCD_BL, 0);
    ledcWrite(0, 255);
#endif

    gfx->begin();
    gfx->fillScreen(PINK);

#if defined DO0143FAT01
    // DO0143FAT01
    for (uint8_t i = 0; i < 255; i++)
    {
        gfx->Display_Brightness(i);
        delay(3);
    }
#endif

#if defined DXQ120MYB2416A
    // DXQ120MYB2416A
    for (uint8_t i = 0; i < 255; i++)
    {
        gfx->Display_Brightness(i);
        delay(3);
    }
#endif

    gfx->draw24bitRGBBitmap(0, 0, gImage_1, LCD_WIDTH, LCD_HEIGHT); // RGB
    delay(1000);

    gfx->draw24bitRGBBitmap(0, 0, gImage_2, LCD_WIDTH, LCD_HEIGHT); // RGB
    delay(1000);

    // gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_1, LCD_WIDTH, LCD_HEIGHT); // RGB
    // delay(1000);

    // gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_2, LCD_WIDTH, LCD_HEIGHT); // RGB
    // delay(1000);
}

void loop()
{
    gfx->draw24bitRGBBitmap(0, 0, gImage_1, LCD_WIDTH, LCD_HEIGHT); // RGB
    delay(1000);

    gfx->draw24bitRGBBitmap(0, 0, gImage_2, LCD_WIDTH, LCD_HEIGHT); // RGB
    delay(1000);

    // gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_1, LCD_WIDTH, LCD_HEIGHT); // RGB
    // delay(1000);

    // gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_2, LCD_WIDTH, LCD_HEIGHT); // RGB
    // delay(1000);
}