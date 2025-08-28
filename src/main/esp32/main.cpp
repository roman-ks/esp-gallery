#include "main.h"
#include "LittleFS.h"
#include <vector>
#include "PNG_support.h"
#include "thumbnail_utils.h"
#include "mbedtls/base64.h"
#include "../core/configs.h"

// Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

short imageNum = 0;
std::vector<char*> images;
uint8_t highlightIndex = 0;

void showPng(IMG_HOLDER imgHolder, uint16_t x, uint16_t y);
bool isButtonPressed();
unsigned char* encodeBase64(const uint8_t *input, size_t len);
void showThumbnails(std::vector<char*> thumbnailPaths);
void showThumbnail(int index);
void showThumbnail(char* tnPath, int x, int y);
void goToNextHighlightBox();
void drawHighlightBox();
uint8_t getBoxX(int i);
uint8_t getBoxY(int i);

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

  Serial.printf("Max cols: %d, max rows: %d\n", GRID_MAX_COLS, GRID_MAX_ROWS);

  Serial.println("\r\nInitialisation done.");
  showThumbnails(images);
  drawHighlightBox();
}

void showThumbnails(std::vector<char*> thumbnailPaths){
  unsigned long start = micros();
  int thumbnailX=0, thumbnailY=0;
  for (size_t i = 0; i < thumbnailPaths.size(); i++){
    showThumbnail(i);
  }
  Serial.printf("Showing %d thumbnails\n", thumbnailPaths.size());

}

uint8_t getBoxX(int i){
  return (i % GRID_MAX_COLS) * GRID_ELEMENT_WIDTH;
}

uint8_t getBoxY(int i){
  return (i / GRID_MAX_COLS) * GRID_ELEMENT_HEIGHT;
}

void showThumbnail(int i){
  showThumbnail(images[i], getBoxX(i), getBoxY(i));
}

void showThumbnail(char* tnPath, int x, int y){
    Serial.printf("Creating thumbnail for %s\n", tnPath);
    IMG_HOLDER imageHolder = readPngIntoArray(tnPath);
    IMG_HOLDER thumbnailHolder = createThumbnailNearest(&imageHolder);
    free(imageHolder.imageBytes);
    showPng(thumbnailHolder, x, y);
}
 
void loop() {

  if(isButtonPressed()){
    tft.fillRect(getBoxX(highlightIndex), getBoxY(highlightIndex), GRID_ELEMENT_WIDTH, GRID_ELEMENT_HEIGHT, TFT_BLACK);
    showThumbnail(highlightIndex);
    goToNextHighlightBox();
    drawHighlightBox();
    // imageNum++;
    // if(imageNum>=images.size())
    //   imageNum=0;
    // tft.fillScreen(0x00);
    // showPng(readPngIntoArray(images[imageNum]), 0, 0);
    // Serial.printf("Showing image #%d: %s\n", imageNum, images[imageNum]);

  }
  delay(100);

}

void goToNextHighlightBox(){
  highlightIndex++;
  Serial.printf("Hl index %d\n", highlightIndex);
 
  
  if(highlightIndex >= images.size()){
    Serial.println("Resetting highlight index");
    highlightIndex=0;
  }
}

void drawHighlightBox(){
  int8_t thickness = 2;
  uint8_t highlighX = getBoxX(highlightIndex);
  uint8_t highlighY = getBoxY(highlightIndex);
  // vertical lines
  tft.fillRect(highlighX, highlighY, thickness, GRID_ELEMENT_HEIGHT, TFT_BLUE);  
  tft.fillRect(highlighX+GRID_ELEMENT_WIDTH-thickness, highlighY, thickness, GRID_ELEMENT_HEIGHT, TFT_BLUE);
  // horizontal lines
  tft.fillRect(highlighX+thickness, highlighY, GRID_ELEMENT_WIDTH-2*thickness, thickness, TFT_BLUE);  
  tft.fillRect(highlighX+thickness, highlighY+GRID_ELEMENT_HEIGHT-thickness, GRID_ELEMENT_WIDTH-2*thickness, thickness, TFT_BLUE);
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

