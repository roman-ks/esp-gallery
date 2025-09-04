#include "main.h"
#include <Arduino.h>
#include "image/gif_image.h"
// #include "LittleFS.h"
// #include <vector>
// #include "mbedtls/base64.h"
// #include <AnimatedGIF.h>
#include "controller.h"
#include "gallery.h"
#include "renderer/tft_renderer.h"

// // Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

// Renderer renderer = TFTRenderer(tft);
// Gallery gallery(renderer, tft);
// Controller controller(gallery);


void setup() {

//   Serial.begin(115200);

//   if(!psramFound()){
//     Serial.println("PSRAM not found!");
//     while(0);
//   }

//   if(!LittleFS.begin()){
//     Serial.println("An Error has occurred while mounting LittleFS");
//     return;
//   }
  
//   tft.begin();
//   controller.init();
//   gallery.init();

//   // gif.begin(BIG_ENDIAN_PIXELS);
//   Serial.println("\r\nInitialisation done.");
}

void loop() {
//   controller.loop();
//   // put your main code here, to run repeatedly:
//   // gif.playFrame(true, NULL);
//   // delay(100);
}

// // unsigned char* encodeBase64(const uint8_t *input, size_t len){
// //   uint32_t base64length = 4*((len+2)/3)+1;
// //   Serial.printf("Expected base64 size: %d\n", base64length);
// //   unsigned char *encoded = (unsigned char*)ps_malloc(base64length);
// //   Serial.printf("Allocated base64 buffer at 0x%x\n", encoded);
// //   size_t outLen;
// //   mbedtls_base64_encode(encoded, base64length, &outLen, input, len);
// //   // terminate to use as c-style string
// //   encoded[outLen]=0;
// //   Serial.println("Base64 encoding finished");
// //   return encoded;
// // }

