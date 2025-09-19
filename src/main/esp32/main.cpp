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
#include "renderer/renderer_cache.h"

// // Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

std::unique_ptr<RendererCache> rendererCache; 
std::unique_ptr<Renderer> renderer;
std::unique_ptr<Gallery> gallery;
std::unique_ptr<Controller> controller;

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
  
  rendererCache = std::make_unique<RendererCache>(LittleFS); 
  renderer = std::make_unique<TFTRenderer>(tft, *rendererCache);
  gallery = std::make_unique<Gallery>(*renderer);
  controller = std::make_unique<Controller>(*gallery);

  tft.begin();
  renderer->init();
  controller->init();
  gallery->init();
}

void loop() {
  controller->loop();
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

