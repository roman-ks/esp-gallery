#define ESP_S3_BOARD XIAO

#if defined(ESP_S3_BOARD) && ESP_S3_BOARD == XIAO
// for ESP32S3 XIAO Seeed board

// tft pins have to be copied to User_Setup.h, these defines are not used in lib
#define TFT_MISO  D9
#define TFT_MOSI  D10 // DIN
#define TFT_SCLK  D8
#define TFT_CS    D0  // Chip select control pin
#define TFT_DC    D1  // Data Command control pin
#define TFT_RST   D2  // Reset pin (could connect to RST pin)

#define SCROLL_RIGHT_BUTTON 5
#define ENTER_BUTTON        6
#define BUTTON_INDEX_OFFSET 5

#define SD_CS    4

#else

// for ESP32S3 Dev board
#define SCROLL_RIGHT_BUTTON 5
#define ENTER_BUTTON 4
#define BUTTON_INDEX_OFFSET 4

#endif