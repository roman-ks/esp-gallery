#include "main.h"
#include <Arduino.h>
#include "image/gif_image.h"
#include "LittleFS.h"
// #include <vector>
// #include "mbedtls/base64.h"
#include "controller.h"
#include "gallery.h"
#include "renderer/tft_renderer.h"
#include <memory>
#include "log.h"

// // Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

std::unique_ptr<Renderer> renderer = std::make_unique<TFTRenderer>(tft);
Gallery gallery(*renderer);
Controller controller(gallery);

Image *img;

#include "image/image_factory.h"
void setup() {

  Serial.begin(115200);

  if(!psramFound()){
    LOG("PSRAM not found!");
    while(0);
  }

  if(!LittleFS.begin()){
    LOG("An Error has occurred while mounting LittleFS");
    return;
  }
  
  tft.begin();
  renderer->init();
  controller.init();
  gallery.init();

  // img = ImageFactory::createDownscaledImage("/dotpict_a_20250831_125038.gif");
  // img->setPosition(80, 80);
  // img->render(*renderer);
}

void loop() {
  // img->render(*renderer);

  controller.loop();
}

// // unsigned char* encodeBase64(const uint8_t *input, size_t len){
// //   uint32_t base64length = 4*((len+2)/3)+1;
// //   LOGF("Expected base64 size: %d\n", base64length);
// //   unsigned char *encoded = (unsigned char*)ps_malloc(base64length);
// //   LOGF("Allocated base64 buffer at 0x%x\n", encoded);
// //   size_t outLen;
// //   mbedtls_base64_encode(encoded, base64length, &outLen, input, len);
// //   // terminate to use as c-style string
// //   encoded[outLen]=0;
// //   LOG("Base64 encoding finished");
// //   return encoded;
// // }

