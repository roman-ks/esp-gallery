#include "LittleFS.h"
#include <vector>
// Include the PNG decoder library
#include <PNGdec.h>
#include "PNG_support.h"


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
    int16_t rc = png.open(filepath, &pngOpen, pngClose, pngRead, pngSeek, pngDraw);
    if (rc == PNG_SUCCESS) {
        tft.startWrite();
        // Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
        uint32_t dt = millis();
        if (png.getWidth() > MAX_IMAGE_WIDTH) {
          Serial.println("Image too wide for allocated line buffer size!");
        } else {
          rc = png.decode(NULL, 0);
          png.close();
        }
        tft.endWrite();
        // How long did rendering take...
        // Serial.print(millis()-dt); Serial.println("ms");
      }
}

// default to HIGH
int oldButtonState=1;

bool isButtonPressed(){
  int buttonState = digitalRead(5);
  bool pressed = false;
  if(oldButtonState==1 && buttonState == 0){
    pressed = true;
  }
  oldButtonState = buttonState;
  return pressed;
}


//=========================================v==========================================
//                                      pngDraw
//====================================================================================
// This next function will be called during decoding of the png file to
// render each image line to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
// Callback function to draw pixels to the display
int pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
  return 1;
}

