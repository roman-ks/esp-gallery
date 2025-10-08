#include "main.h"
#include <Arduino.h>
#include "image/gif_image.h"
#include "SD.h"
// #include <vector>
// #include "mbedtls/base64.h"
#include "gallery_controller.h"
#include "main_controller.h"
#include "gallery.h"
#include "renderer/tft_renderer.h"
#include <memory>
#include "log.h"
#include "renderer/renderer_cache.h"
#include "pins_config.h"
#include "utils/sd_init.h"
#include <vector>

// // Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

std::unique_ptr<RendererCache> rendererCache; 
std::unique_ptr<Renderer> renderer;
std::unique_ptr<Gallery> gallery;
std::unique_ptr<MainController> mainController;

void setup() {

  Serial.begin(115200);

  if(!psramFound()){
    LOG_W("PSRAM not found!");
    while(1);
  }

  // TFT has to start first because it sets up the SPI bus
  tft.begin();
  tft.fillScreen(TFT_BLACK);

  // SD card uses same SPI so take clock from TFT
  SdIniter sdIniter(TFT_SCLK, SD_CS, tft.getSPIinstance());
  if(!sdIniter.init()){
    // todo write to TFT
    LOG_W("SD Card Mount Failed");
    while(1);
  }
  
  fs::FS &fileSys = SD;
  rendererCache = std::make_unique<RendererCache>(fileSys); 
  renderer = std::make_unique<TFTRenderer>(tft, *rendererCache, fileSys);
  gallery = std::make_unique<Gallery>(*renderer, *rendererCache, fileSys);
  std::vector<std::unique_ptr<IController>> controllers;
  controllers.push_back(std::make_unique<GalleryController>(*gallery));
  mainController = std::make_unique<MainController>(std::move(controllers));

  renderer->init();
  mainController->init();
  gallery->init();
}

void loop() {
  mainController->loop();
}

