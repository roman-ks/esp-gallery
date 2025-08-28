#include "main.h"
#include "LittleFS.h"
#include <vector>
// Include the PNG decoder library
#include "PNG_support.h"
#include "thumbnail_utils.h"
#include "mbedtls/base64.h"

#define GRID_ELEMENT_HEIGHT 80
#define GRID_ELEMENT_WIDTH 80

#define MAX_IMAGE_WIDTH 240 // Adjust for your images

int16_t xpos = 0;
int16_t ypos = 0;

// Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

short imageNum = 0;
std::vector<char*> images;

void showPng(IMG_HOLDER imgHolder, uint16_t x, uint16_t y);
bool isButtonPressed();
unsigned char* encodeBase64(const uint8_t *input, size_t len);
void showThumbnails(std::vector<char*> thumbnailPaths);

void setup() {
  pinMode(5, INPUT_PULLUP);

  Serial.begin(115200);

  if(!psramFound()){
    Serial.println("PSRAM not found!");
    while(0);
  }

  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  
  File root = LittleFS.open("/", "r");
  while(File file = root.openNextFile()){
    char buf[1000];
    snprintf(buf, sizeof(buf), "/%s", file.name());
    Serial.println(buf);
    if(String(buf).endsWith(".png")){
      images.push_back(strdup(buf));
    }
  }
  Serial.printf("Found %d images\n", images.size());
  
  // Initialise the TFT
  tft.begin();
  tft.fillScreen(TFT_BLACK);

  Serial.println("\r\nInitialisation done.");
  showThumbnails(images);
}

void showThumbnails(std::vector<char*> thumbnailPaths){
  unsigned long start = micros();
  int x=0, y=0;
  for(auto tnPath: thumbnailPaths){
    Serial.printf("Creating thumbnail for %s\n", tnPath);
    IMG_HOLDER imageHolder = readPngIntoArray(tnPath);
    IMG_HOLDER thumbnailHolder = createThumbnailNearest(&imageHolder);
    free(imageHolder.imageBytes);
    showPng(thumbnailHolder, x, y);
    x += GRID_ELEMENT_WIDTH;
    if (x + GRID_ELEMENT_WIDTH > tft.width()){
      x = 0;
      y += GRID_ELEMENT_HEIGHT;
      if( y + GRID_ELEMENT_HEIGHT>tft.height())
        break;
    }
  }
  Serial.printf("Showing %d thumbnails\n", thumbnailPaths.size());

}
 
void loop() {

  if(isButtonPressed()){
    imageNum++;
    if(imageNum>=images.size())
      imageNum=0;
    tft.fillScreen(0x00);
    showPng(readPngIntoArray(images[imageNum]), 0, 0);
    Serial.printf("Showing image #%d: %s\n", imageNum, images[imageNum]);

  }
  delay(100);

}

void showPng(IMG_HOLDER imageHolder, uint16_t x, uint16_t y){
  unsigned long start = micros();
  tft.startWrite();
  tft.pushImage(x,y, imageHolder.width, imageHolder.height, imageHolder.imageBytes);
  tft.endWrite();
  Serial.println("Finished drawing");
  free(imageHolder.imageBytes);
  Serial.println("Freed buffer");
}

// default to HIGH
int oldButtonState=HIGH;

bool isButtonPressed(){
  int buttonState = digitalRead(5);
  bool pressed = false;
  if(oldButtonState==HIGH && buttonState == LOW){
    pressed = true;
  }
  oldButtonState = buttonState;
  return pressed;
}

unsigned char* encodeBase64(const uint8_t *input, size_t len){
  uint32_t base64length = 4*((len+2)/3)+1;
  Serial.printf("Expected base64 size: %d\n", base64length);
  unsigned char *encoded = (unsigned char*)ps_malloc(base64length);
  Serial.printf("Allocated base64 buffer at 0x%x\n", encoded);
  size_t outLen;
  mbedtls_base64_encode(encoded, base64length, &outLen, input, len);
  // terminate to use as c-style string
  encoded[outLen]=0;
  Serial.println("Base64 encoding finished");
  return encoded;
}

