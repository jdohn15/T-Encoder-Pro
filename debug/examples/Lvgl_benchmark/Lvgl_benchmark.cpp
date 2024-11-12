/*
 * @Description:
    LVGL Configuration file:
    Copy your_arduino_path/libraries/lvgl/lv_conf_template.h
    to your_arduino_path/libraries/lv_conf.h

    In lv_conf.h around line 15, enable config file:
    #if 1 // Set it to "1" to enable content

    Then find and set:
    #define LV_COLOR_DEPTH     16
    #define LV_TICK_CUSTOM     1

    For SPI/parallel 8 display set color swap can be faster, parallel 16/RGB screen don't swap!
    #define LV_COLOR_16_SWAP   1 // for SPI and parallel 8
    #define LV_COLOR_16_SWAP   0 // for parallel 16 and RGB

 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-09-22 11:59:37
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-09-23 16:18:21
 * @License: GPL 3.0
 */
#include "lvgl.h"
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include "lv_demo_benchmark.h"
#include "TouchDrvCHSC5816.hpp"

// 加载图片声明 .c文件的图片
LV_IMG_DECLARE(img_1);
LV_IMG_DECLARE(img_2);
LV_IMG_DECLARE(img_3);

static uint32_t screenWidth = LCD_WIDTH;   // screenWidth
static uint32_t screenHeight = LCD_HEIGHT; // screenHeight

uint8_t Img_Mode = 0;
bool Img_Change = false;

// DXQ120MYB2416A
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */,
    LCD_SDIO1 /* SDIO1 */, LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST /* RST */, 0 /* rotation */,
                                      false /* IPS */, LCD_WIDTH, LCD_HEIGHT);

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

TouchDrvCHSC5816 touch;
TouchDrvInterface *pTouch;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

    lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    int16_t Touch_x[2], Touch_y[2];
    uint8_t touchpad = touch.getPoint(Touch_x, Touch_y);

    if (touchpad > 0)
    {
        data->state = LV_INDEV_STATE_PR;

        if (data->point.x != Touch_x[0] && data->point.y != Touch_y[0])
        {
            Img_Change = true;
        }

        /*Set the coordinates*/
        data->point.x = Touch_x[0];
        data->point.y = Touch_y[0];

        // Serial.print("Data x ");
        // Serial.printf("%d\n", x[0]);

        // Serial.print("Data y ");
        // Serial.printf("%d\n", y[0]);
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

/**
 * @brief 样式背景图片的显示
 * @return
 * @Date 2023-06-12 11:20:32
 */
void lv_style_img(uint8_t img)
{
    static lv_style_t style;         // 创建样式
    lv_style_init(&style);           // 初始化样式
    lv_style_set_radius(&style, 10); // 设置圆角半径

    switch (img)
    {
    case 1:
        lv_style_set_bg_img_src(&style, &img_1); // 设置背景图片
        break;
    case 2:
        lv_style_set_bg_img_src(&style, &img_2); // 设置背景图片
        break;
    case 3:
        lv_style_set_bg_img_src(&style, &img_3); // 设置背景图片
        break;

    default:
        break;
    }

    lv_style_set_bg_img_opa(&style, LV_OPA_COVER); // 设置背景图片透明度
    // lv_style_set_bg_img_recolor(&style, lv_palette_main(LV_PALETTE_INDIGO)); // 设置背景图片重着色
    // lv_style_set_bg_img_recolor_opa(&style, LV_OPA_20);                      // 设置背景图片重着色透明度
    lv_style_set_bg_img_tiled(&style, false); // 设置背景图片平铺

    lv_style_set_x(&style, lv_pct(0)); // 设置样式的x位置 ，基准点左上角
    lv_style_set_y(&style, 0);         // 设置样式的y位置

    /*Create an object with the new style*/
    lv_obj_t *obj = lv_obj_create(lv_scr_act()); // 创建对象
    lv_obj_add_style(obj, &style, 0);            // 设置对象的样式
    lv_obj_set_size(obj, 390, 390);              // 设置对象的尺寸

    // lv_obj_center(obj);                          // 居中对象
}

void CHSC5816_Initialization(void)
{
    TouchDrvCHSC5816 *pd1 = static_cast<TouchDrvCHSC5816 *>(pTouch);

    touch.setPins(TOUCH_RST, TOUCH_INT);
    if (!touch.begin(Wire, CHSC5816_SLAVE_ADDRESS, IIC_SDA, IIC_SCL))
    {
        Serial.println("Failed to find CHSC5816 - check your wiring!");
        while (1)
        {
            delay(1000);
        }
    }

    Serial.println("Init CHSC5816 Touch device success!");
}

void lvgl_initialization(void)
{
    lv_init();

    screenWidth = gfx->width();
    screenHeight = gfx->height();

    disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * 40, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    while (!disp_draw_buf)
    {
        Serial.println("LVGL disp_draw_buf allocate failed!");
        delay(1000);
    }

    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * 40);

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
}

void setup()
{
    Serial.begin(115200);

    // DXQ120MYB2416A
    pinMode(LCD_VCI_EN, OUTPUT);
    digitalWrite(LCD_VCI_EN, HIGH);

    CHSC5816_Initialization();

    // Init Display
    if (!gfx->begin())
    {
        Serial.println("gfx->begin() failed!");
    }
    gfx->fillScreen(PINK);

    // DXQ120MYB2416A
    for (int i = 0; i < 255; i++)
    {
        gfx->Display_Brightness(i);
        delay(3);
    }

    lvgl_initialization();
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);

    if (Img_Change == true)
    {
        Img_Change = false;
        Img_Mode++;

        if (Img_Mode <= 3)
        {
            lv_style_img(Img_Mode);
        }
        if (Img_Mode >= 4)
        {
            lv_style_img(4);
            lv_demo_benchmark();
            Img_Mode = 0;
        }
    }
}