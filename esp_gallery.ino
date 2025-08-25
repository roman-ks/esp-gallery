#include "LittleFS.h"
#include <vector>
// Include the PNG decoder library
#include <PNGdec.h>
#include "PNG_support.h"
#include "thumbnail_utils.h"


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
  IMG_HOLDER imageHolder = readPngIntoArray(filepath);
  tft.startWrite();
  tft.pushImage(0,0, imageHolder.width, imageHolder.height, imageHolder.imageBytes);
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

