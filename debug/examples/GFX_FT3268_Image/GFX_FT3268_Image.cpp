/*
 * @Description: None
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-09-06 10:58:19
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-11-22 16:01:15
 * @License: GPL 3.0
 */

// #define TOUCH_MODULES_GT911
// #define TOUCH_MODULES_CST_SELF
// #define TOUCH_MODULES_CST_MUTUAL
// #define TOUCH_MODULES_ZTW622
// #define TOUCH_MODULES_L58
#define TOUCH_MODULES_FT3267
// #define TOUCH_MODULES_FT5x06

#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include "TouchLib.h"
#include "Wire.h"
#include "material_16Bit.h"

#define PIN_IIC_SCL 6
#define PIN_IIC_SDA 5
#define PIN_TOUCH_INT 9
#define PIN_TOUCH_RES 8

// NFP099B-01A
// Arduino_DataBus *bus = new Arduino_SWSPI(
//     LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */);

// Arduino_GFX *gfx = new Arduino_GC9107(
//     bus, LCD_RST /* RST */, 0 /* rotation */, true /* IPS */,
//     LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
//     0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

// DO0143FAT01
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */, LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST /* RST */, 0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);

TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, FT3267_SLAVE_ADDRESS);

bool get_int = false;
bool image_flag = 0;

void scan_i2c_device(TwoWire &i2c)
{
    Serial.println("Scanning for I2C devices ...");
    Serial.print("      ");
    for (int i = 0; i < 0x10; i++)
    {
        Serial.printf("0x%02X|", i);
    }
    uint8_t error;
    for (int j = 0; j < 0x80; j += 0x10)
    {
        Serial.println();
        Serial.printf("0x%02X |", j);
        for (int i = 0; i < 0x10; i++)
        {
            Wire.beginTransmission(i | j);
            error = Wire.endTransmission();
            if (error == 0)
                Serial.printf("0x%02X|", i | j);
            else
                Serial.print(" -- |");
        }
    }
    Serial.println();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(PIN_TOUCH_RES, OUTPUT);
    digitalWrite(PIN_TOUCH_RES, 0);
    delay(200);
    digitalWrite(PIN_TOUCH_RES, 1);
    delay(200);

    attachInterrupt(
        PIN_TOUCH_INT,
        []
        {
            get_int = 1;
            Serial.println("get_int");
        },
        FALLING);

    Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL);
    // scan_i2c_device(Wire);

    touch.init();

    gfx->begin();

    gfx->fillScreen(PINK);

    // DO0143FAT01
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

    gfx->setCursor(233, 233);
    gfx->printf("Ciallo");
}

void loop()
{
    if (get_int)
    {
        get_int = 0;
        touch.read();
        uint8_t n = touch.getPointNum();
        TP_Point t = touch.getPoint(0);
        gfx->setCursor(t.x, t.y);
        if (n > 0)
        {
            // gfx->fillScreen(PINK);
            Serial.printf("[1] point x: %d  point y: %d \r\n", t.x, t.y);
            gfx->printf("[1] point x: %d  point y: %d \r\n", t.x, t.y);
        }
        else
        {
            if (image_flag == 0)
            {
                gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_1, LCD_WIDTH, LCD_HEIGHT); // RGB
            }
            if (image_flag == 1)
            {
                gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_2, LCD_WIDTH, LCD_HEIGHT); // RGB
            }
            image_flag = !image_flag;
            Serial.printf("[1] point x: %d  point y: %d \r\n", t.x, t.y);
            gfx->printf("[1] point x: %d  point y: %d \r\n", t.x, t.y);
        }
    }
}
