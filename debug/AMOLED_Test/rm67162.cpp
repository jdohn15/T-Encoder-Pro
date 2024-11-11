#include "rm67162.h"
#include "SPI.h"
#include "Arduino.h"
#include "driver/spi_master.h"

const static lcd_cmd_t rm67162_spi_init[] = {
    {0xFE, {0x00}, 0x01}, // PAGE
    // {0x35, {0x00},        0x00}, //TE ON
    // {0x34, {0x00},        0x00}, //TE OFF
    {0x36, {0x00}, 0x01}, // Scan Direction Control
    {0x3A, {0x75}, 0x01}, // Interface Pixel Format	16bit/pixel
    // {0x3A, {0x76},        0x01}, //Interface Pixel Format	18bit/pixel
    // {0x3A, {0x77},        0x01}, //Interface Pixel Format	24bit/pixel
    {0x51, {0x00}, 0x01},        // Write Display Brightness MAX_VAL=0XFF
    {0x11, {0x00}, 0x01 | 0x80}, // Sleep Out
    {0x29, {0x00}, 0x01 | 0x80}, // Display on
    {0x51, {0xD0}, 0x01},        // Write Display Brightness	MAX_VAL=0XFF
};

const static lcd_cmd_t rm67162_qspi_init[] = {
    {0x11, {0x00}, 0}, // Sleep Out
    {0x00, {0x00}, 255},
    // {0x13, {0x00}, 0},       //Normal Display mode on
    // {0x23, {0x00}, 0},       //All pixels on
    {0x44, {0x01, 0xD1}, 2}, // Set_Tear_Scanline
    {0x35, {0x00}, 1},       // TE ON
    // {0x34, {0x00}, 0x00}, // TE OFF
    // {0x36, {0x00},        0x01}, //Scan Direction Control
    {0x3A, {0x55}, 1}, // Interface Pixel Format 16bit/pixel
                       // {0x3A, {0x66},        0x01}, //Interface Pixel Format    18bit/pixel
                       // {0x3A, {0x77},        0x01}, //Interface Pixel Format    24bit/pixel
    {0x53, {0x20}, 1},
    {0x51, {0x00}, 1}, // Write Display Brightness MAX_VAL=0XFF
    {0x29, {0x00}, 0}, // Display on
    {0x51, {0xFF}, 1}, // Write Display Brightness   MAX_VAL=0XFF
};

static spi_device_handle_t spi;

void Chip_Scan(void)
{
    uint64_t chipid;

    chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).

    Serial.printf("\n------------------------------------------------\n");

    Serial.printf("ESP32 Chip ID = %04X", (uint16_t)(chipid >> 32)); // print High 2 bytes
    Serial.printf("%08X\n", (uint32_t)chipid);                       // print Low 4bytes.
    Serial.printf("Chip Revision = %u\n", ESP.getChipRevision());
    Serial.printf("CpuFreqMHz = %u MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Cycle Count = %u\n", ESP.getCycleCount());
    Serial.printf("SdkVersion = %s\n", ESP.getSdkVersion());

    Serial.printf("\n");

    Serial.printf("Total heap size = %u B\n", ESP.getHeapSize());
    Serial.printf("Available heap = %u B\n", ESP.getFreeHeap());
    Serial.printf("Lowest level of free heap since boot = %u B\n", ESP.getMinFreeHeap());
    Serial.printf("Largest block of heap that can be allocated at once = %u B\n", ESP.getMaxAllocHeap());

    Serial.printf("\n");

    Serial.printf("Total Psram size = %u B\n", ESP.getPsramSize());
    Serial.printf("Available Psram = %u B\n", ESP.getFreePsram());
    Serial.printf("Lowest level of free Psram since boot = %u B\n", ESP.getMinFreePsram());
    Serial.printf("Largest block of Psram that can be allocated at once = %u B\n", ESP.getMaxAllocPsram());

    Serial.printf("\n");

    Serial.printf("Flash chip size: %u B\n", ESP.getFlashChipSize());
    Serial.printf("Flash chip speed: %u Hz\n", ESP.getFlashChipSpeed());

    switch (ESP.getFlashChipMode())
    {
    case FM_QIO:
        Serial.printf("Flash chip mode: FM_QIO \n");
        break;
    case FM_QOUT:
        Serial.printf("Flash chip mode: FM_QOUT \n");
        break;
    case FM_DIO:
        Serial.printf("Flash chip mode: FM_DIO \n");
        break;
    case FM_DOUT:
        Serial.printf("Flash chip mode: FM_DOUT \n");
        break;
    case FM_FAST_READ:
        Serial.printf("Flash chip mode: FM_FAST_READ \n");
        break;
    case FM_SLOW_READ:
        Serial.printf("Flash chip mode: FM_SLOW_READ \n");
        break;
    case FM_UNKNOWN:
        Serial.printf("Flash chip mode: FM_UNKNOWN \n");
        break;

    default:
        break;
    }

    Serial.printf("------------------------------------------------\n");
}

static void WriteComm(uint8_t data)
{
    TFT_CS_L;
    SPI.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, TFT_SPI_MODE));
    // TFT_DC_L;
    SPI.write(data);
    // TFT_DC_H;
    SPI.endTransaction();
    TFT_CS_H;
}

static void WriteData(uint8_t data)
{
    TFT_CS_L;
    SPI.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, TFT_SPI_MODE));
    // TFT_DC_H;
    SPI.write(data);
    SPI.endTransaction();
    TFT_CS_H;
}

static void WriteData16(uint16_t data)
{

    TFT_CS_L;
    SPI.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, TFT_SPI_MODE));
    // TFT_DC_H;
    SPI.write16(data);
    SPI.endTransaction();
    TFT_CS_H;
}

static void lcd_send_cmd(uint32_t cmd, uint8_t *dat, uint32_t len)
{
#if LCD_USB_QSPI_DREVER == 1
    TFT_CS_L;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.flags = (SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR);
    t.cmd = 0x02;
    t.addr = cmd << 8;
    // Serial.printf("t.addr:0x%X\r\n", t.addr);
    if (len != 0)
    {
        t.tx_buffer = dat;
        t.length = 8 * len;
    }
    else
    {
        t.tx_buffer = NULL;
        t.length = 0;
    }
    spi_device_polling_transmit(spi, &t);
    TFT_CS_H;
#else
    WriteComm(cmd);
    if (len != 0)
    {
        for (int i = 0; i < len; i++)
            WriteData(dat[i]);
    }
#endif
}

void rm67162_init(void)
{
    pinMode(TFT_QSPI_CS, OUTPUT);
    pinMode(TFT_QSPI_RST, OUTPUT);

    digitalWrite(TFT_QSPI_RST, HIGH);
    delay(10);
    digitalWrite(TFT_QSPI_RST, LOW);
    delay(200);
    digitalWrite(TFT_QSPI_RST, HIGH);
    delay(200);

#if LCD_USB_QSPI_DREVER == 1
    esp_err_t ret;

    spi_bus_config_t buscfg = {
        .data0_io_num = TFT_QSPI_D0,
        .data1_io_num = TFT_QSPI_D1,
        .sclk_io_num = TFT_QSPI_SCK,
        .data2_io_num = TFT_QSPI_D2,
        .data3_io_num = TFT_QSPI_D3,
        .max_transfer_sz = (SEND_BUF_SIZE * 16) + 8,
        .flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_GPIO_PINS /* |
                 SPICOMMON_BUSFLAG_QUAD */
        ,
    };
    spi_device_interface_config_t devcfg = {
        .command_bits = 8,
        .address_bits = 24,
        .mode = TFT_SPI_MODE,
        .clock_speed_hz = SPI_FREQUENCY,
        .spics_io_num = -1,
        // .spics_io_num = TFT_QSPI_CS,
        .flags = SPI_DEVICE_HALFDUPLEX,
        .queue_size = 17,
    };
    ret = spi_bus_initialize(TFT_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    ret = spi_bus_add_device(TFT_SPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

#else
    SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);
    SPI.setFrequency(SPI_FREQUENCY);
    pinMode(TFT_DC, OUTPUT);
#endif
    // Initialize the screen multiple times to prevent initialization failure
    int i = 3;
    while (i--)
    {
#if LCD_USB_QSPI_DREVER == 1
        const lcd_cmd_t *lcd_init = rm67162_qspi_init;
        for (int i = 0; i < sizeof(rm67162_qspi_init) / sizeof(lcd_cmd_t); i++)
#else
        const lcd_cmd_t *lcd_init = rm67162_spi_init;
        for (int i = 0; i < sizeof(rm67162_spi_init) / sizeof(lcd_cmd_t); i++)
#endif
            if (lcd_init[i].len == 255)
            {
                delay(120);
            }
            else
            {
                lcd_send_cmd(lcd_init[i].cmd,
                             (uint8_t *)lcd_init[i].data,
                             lcd_init[i].len);
            }
    }
}

void lcd_setRotation(uint8_t r)
{
    uint8_t gbr = TFT_MAD_RGB;

    switch (r)
    {
    case 0: // Portrait
        // WriteData(gbr);
        break;
    case 1: // Landscape (Portrait + 90)
        gbr = TFT_MAD_MX | TFT_MAD_MV | gbr;
        break;
    case 2: // Inverter portrait
        gbr = TFT_MAD_MX | TFT_MAD_MY | gbr;
        break;
    case 3: // Inverted landscape
        gbr = TFT_MAD_MV | TFT_MAD_MY | gbr;
        break;
    }
    lcd_send_cmd(TFT_MADCTL, &gbr, 1);
}

void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    lcd_cmd_t t[3] = {
        {0x2a, {x1 >> 8, x1, x2 >> 8, x2}, 0x04},
        {0x2b, {y1 >> 8, y1, y2 >> 8, y2}, 0x04},
        {0x2c, {0x00}, 0x00},
    };

    for (uint32_t i = 0; i < 3; i++)
    {
        lcd_send_cmd(t[i].cmd, t[i].data, t[i].len);
    }
}

void lcd_fill(uint16_t xsta,
              uint16_t ysta,
              uint16_t xend,
              uint16_t yend,
              uint16_t color)
{

    for (int i = 0; i < 2; i++)
    {
        uint16_t w = (xend + i * xend) - (xsta + i * xend);
        uint16_t h = yend - ysta;
        // Serial.printf(".esp_get_free_heap_size = %d\n\r", esp_get_free_heap_size());
        Chip_Scan();
        uint16_t *color_p = (uint16_t *)heap_caps_malloc(w * h * 2, MALLOC_CAP_INTERNAL);
        memset(color_p, color, w * h * 2);
        lcd_PushColors((xsta + i * xend), ysta, w, h, color_p);
        // Serial.printf(".esp_get_free_heap_size = %d\n\r", esp_get_free_heap_size());
        Chip_Scan();
        free(color_p);
    }
}

// void AMOLED_fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t high, uint16_t color)
// {
//     bool lock_flag = 0;

//     lcd_address_set(x, y, x + width - 1, y + high - 1);
//     TFT_CS_L;

//     spi_transaction_ext_t t = {0};
//     memset(&t, 0, sizeof(t));
//     if (lock_flag)
//     {
//         t.base.flags =
//             SPI_TRANS_MODE_QIO /* | SPI_TRANS_MODE_DIOQIO_ADDR */;
//         t.base.cmd = 0x32; // or 0x12
//         t.base.addr = 0x002C00;
//         lock_flag = 1;
//     }
//     else
//     {
//         t.base.flags = SPI_TRANS_MODE_QIO | SPI_TRANS_VARIABLE_CMD |
//                        SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_DUMMY;
//         t.command_bits = 0;
//         t.address_bits = 0;
//         t.dummy_bits = 0;
//     }
//     t.base.tx_buffer = tx_data[0];
//     t.base.tx_buffer = tx_data[1];
//     t.base.length = 16;

//     // spi_device_queue_trans(spi, (spi_transaction_t *)&t, portMAX_DELAY);
//     spi_device_polling_transmit(spi, (spi_transaction_t *)&t);

//     TFT_CS_H;
// }

void lcd_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_address_set(x, y, x + 1, y + 1);
    lcd_PushColors(&color, 1);
}

void lcd_PushColors(uint16_t x,
                    uint16_t y,
                    uint16_t width,
                    uint16_t high,
                    uint16_t *data)
{
#if LCD_USB_QSPI_DREVER == 1
    bool first_send = 1;
    size_t len = width * high;
    uint16_t *p = (uint16_t *)data;

    lcd_address_set(x, y, x + width - 1, y + high - 1);
    TFT_CS_L;
    do
    {
        size_t chunk_size = len;
        spi_transaction_ext_t t = {0};
        memset(&t, 0, sizeof(t));
        if (first_send)
        {
            t.base.flags =
                SPI_TRANS_MODE_QIO /* | SPI_TRANS_MODE_DIOQIO_ADDR */;
            t.base.cmd = 0x32 /* 0x12 */;
            t.base.addr = 0x002C00;
            first_send = 0;
        }
        else
        {
            t.base.flags = SPI_TRANS_MODE_QIO | SPI_TRANS_VARIABLE_CMD |
                           SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_DUMMY;
            t.command_bits = 0;
            t.address_bits = 0;
            t.dummy_bits = 0;
        }
        if (chunk_size > SEND_BUF_SIZE)
        {
            chunk_size = SEND_BUF_SIZE;
        }
        t.base.tx_buffer = p;
        t.base.length = chunk_size * 16;

        // spi_device_queue_trans(spi, (spi_transaction_t *)&t, portMAX_DELAY);
        spi_device_polling_transmit(spi, (spi_transaction_t *)&t);
        len -= chunk_size;
        p += chunk_size;
    } while (len > 0);
    TFT_CS_H;

#else
    lcd_address_set(x, y, x + width - 1, y + high - 1);
    TFT_CS_L;
    SPI.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, TFT_SPI_MODE));
    TFT_DC_H;
    SPI.writeBytes((uint8_t *)data, width * high * 2);
    SPI.endTransaction();
    TFT_CS_H;
#endif
}

void lcd_PushColors(uint16_t *data, uint32_t len)
{
#if LCD_USB_QSPI_DREVER == 1
    bool first_send = 1;
    uint16_t *p = (uint16_t *)data;
    TFT_CS_L;
    do
    {
        size_t chunk_size = len;
        spi_transaction_ext_t t = {0};
        memset(&t, 0, sizeof(t));
        if (first_send)
        {
            t.base.flags =
                SPI_TRANS_MODE_QIO /* | SPI_TRANS_MODE_DIOQIO_ADDR */;
            t.base.cmd = 0x32 /* 0x12 */;
            t.base.addr = 0x002C00;
            first_send = 0;
        }
        else
        {
            t.base.flags = SPI_TRANS_MODE_QIO | SPI_TRANS_VARIABLE_CMD |
                           SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_DUMMY;
            t.command_bits = 0;
            t.address_bits = 0;
            t.dummy_bits = 0;
        }
        if (chunk_size > SEND_BUF_SIZE)
        {
            chunk_size = SEND_BUF_SIZE;
        }
        t.base.tx_buffer = p;
        t.base.length = chunk_size * 16;

        // spi_device_queue_trans(spi, (spi_transaction_t *)&t, portMAX_DELAY);
        spi_device_polling_transmit(spi, (spi_transaction_t *)&t);
        len -= chunk_size;
        p += chunk_size;
    } while (len > 0);
    TFT_CS_H;

#else
    TFT_CS_L;
    SPI.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, TFT_SPI_MODE));
    TFT_DC_H;
    SPI.writeBytes((uint8_t *)data, len * 2);
    SPI.endTransaction();
    TFT_CS_H;
#endif
}

void lcd_sleep()
{
    lcd_send_cmd(0x10, NULL, 0);
}

void lcd_brightness(uint8_t bright)
{
    lcd_send_cmd(0x51, &bright, 0x01);
}

void lcd_set_colour_enhance(uint8_t enh)
{
    lcd_send_cmd(0x58, &enh, 0x01);
}

void lcd_display_off()
{
    lcd_send_cmd(0x28, NULL, 0x01);
}

void lcd_display_on()
{
    lcd_send_cmd(0x29, NULL, 0x01);
}

void lcd_display_invert_on()
{
    lcd_send_cmd(0x21, NULL, 0x01);
}

void lcd_display_invert_off()
{
    lcd_send_cmd(0x20, NULL, 0x01);
}

void lcd_display_set_colour_enhance_low_byte(uint8_t ce_low_byte)
{
    lcd_send_cmd(0x5A, &ce_low_byte, 0x01);
}

void lcd_display_set_colour_enhance_high_byte(uint8_t ce_high_byte)
{
    lcd_send_cmd(0x5B, &ce_high_byte, 0x01);
}

void lcd_display_high_brightness_mode_on(uint8_t hbm_en)
{
    lcd_send_cmd(0xB0, &hbm_en, 0x01);
}

void lcd_display_high_brightness_mode_off(uint8_t hbm_en)
{
    lcd_send_cmd(0xB0, &hbm_en, 0x01);
}
