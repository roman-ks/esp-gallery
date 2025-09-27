#define ESP_S3_BOARD XIAO

#if defined(ESP_S3_BOARD) && ESP_S3_BOARD == XIAO
// for ESP32S3 XIAO Seeed board

// tft pins have to be copied to User_Setup.h, this defines dont work here
#define TFT_MISO  -1
#define TFT_MOSI  D10 // DIN
#define TFT_SCLK  D8
#define TFT_CS    D0  // Chip select control pin
#define TFT_DC    D1  // Data Command control pin
#define TFT_RST   D2  // Reset pin (could connect to RST pin)

#define SCROLL_RIGHT_BUTTON 43
#define ENTER_BUTTON 44
#define BUTTON_INDEX_OFFSET 43

#else

// for ESP32S3 Dev board
#define SCROLL_RIGHT_BUTTON 5
#define ENTER_BUTTON 4
#define BUTTON_INDEX_OFFSET 4

#endif