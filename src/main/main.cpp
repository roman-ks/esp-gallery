#include "main.h"
#include "LittleFS.h"
#include <vector>
// Include the PNG decoder library
#include <PNGdec.h>
#include "PNG_support.h"
#include "thumbnail_utils.h"
#include "mbedtls/base64.h"


PNG png;
#define MAX_IMAGE_WIDTH 240 // Adjust for your images

int16_t xpos = 0;
int16_t ypos = 0;

// Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

short imageNum = 0;
std::vector<char*> images;

void showPng(char *filepath);
bool isButtonPressed();
unsigned char* encodeBase64(const uint8_t *input, size_t len);

void setup() {
  pinMode(5, INPUT_PULLUP);

  Serial.begin(115200);

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
  
  showPng(images[imageNum]);
  Serial.printf("Showing image #%d: %s\n", imageNum, images[imageNum]);
}
 
void loop() {

  if(isButtonPressed()){
    imageNum++;
    if(imageNum>=images.size())
      imageNum=0;
    tft.fillScreen(0x00);
    showPng(images[imageNum]);
    Serial.printf("Showing image #%d: %s\n", imageNum, images[imageNum]);

  }
  delay(100);

}

void showPng(char *filepath){
  unsigned long start = micros();
  IMG_HOLDER imageHolder = readPngIntoArray(filepath);
  Serial.printf("Read original img in %f ms\n", (micros()-start)/1000.0f);
  unsigned char* encoded = encodeBase64((uint8_t*)imageHolder.imageBytes, imageHolder.width*imageHolder.height*sizeof(uint16_t));
  Serial.println("Wrinting original image base64");
  Serial.println((char*)encoded);
  free(encoded);
  // start = micros();
  // IMG_HOLDER thumbnailHolder = createThumbnail(&imageHolder);
  // Serial.printf("Int thumbnail created in %f ms\n", (micros()-start)/1000.0f);

  // start = micros();
  // IMG_HOLDER fThumbnailHolder = createThumbnailFloat(&imageHolder);
  // Serial.printf("float thumbnail created in %f ms\n", (micros()-start)/1000.0f);

  IMG_HOLDER nThumbnailHolder = createThumbnailFloat(&imageHolder);
  Serial.printf("Nearest neighbour thumbnail created in %f ms\n", (micros()-start)/1000.0f);
  encoded = encodeBase64((uint8_t*)nThumbnailHolder.imageBytes, nThumbnailHolder.width*nThumbnailHolder.height*sizeof(uint16_t));
  Serial.println("Wrinting nearest neighbours image base64");
  Serial.println((char*)encoded);
  free(encoded);

  tft.startWrite();
  // tft.pushImage(0,0, thumbnailHolder.width, thumbnailHolder.height, thumbnailHolder.imageBytes);
  // tft.pushImage(0,80, fThumbnailHolder.width, fThumbnailHolder.height, fThumbnailHolder.imageBytes);
  tft.pushImage(0,160, nThumbnailHolder.width, nThumbnailHolder.height, nThumbnailHolder.imageBytes);


  tft.endWrite();
  Serial.println("Finished drawing");
  free(imageHolder.imageBytes);
  // free(thumbnailHolder.imageBytes);
  // free(fThumbnailHolder.imageBytes);
  free(nThumbnailHolder.imageBytes);
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

