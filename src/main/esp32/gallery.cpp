#include "gallery.h"

Gallery::Gallery(TFT_eSPI &tft) : tft(tft), images() {
}

Gallery::~Gallery() {
}

void Gallery::init() {
    // Initialise the TFT
    tft.fillScreen(TFT_BLACK);

    fs::File root = LittleFS.open("/", "r");
    while(fs::File file = root.openNextFile()){
        char buf[1000];
        snprintf(buf, sizeof(buf), "/%s", file.name());
        Serial.println(buf);
        if(String(buf).endsWith(".png")){
            images.push_back(strdup(buf));
        }
    }
    
    Serial.printf("Found %d images\n", images.size());  
    Serial.printf("Max cols: %d, max rows: %d\n", GRID_MAX_COLS, GRID_MAX_ROWS);

    showThumbnails(images);
    drawHighlightBox();
}

void Gallery::nextImage(){
    // todo move to some util
    imageIndex++;
    if(imageIndex >= images.size()){
        imageIndex = 0;
    }
    tft.fillScreen(TFT_BLACK);
    showPng(readPngIntoArray(images[imageIndex]), 0, 0);
}

void Gallery::nextHighlight(){
    tft.fillRect(getBoxX(highlightIndex), getBoxY(highlightIndex), GRID_ELEMENT_WIDTH, GRID_ELEMENT_HEIGHT, TFT_BLACK);
    showThumbnail(highlightIndex);
    goToNextHighlightBox();
    drawHighlightBox();
}

bool Gallery::isImageOpen(){
    return imageIndex >= 0;
}

void Gallery::openImage(){
    imageIndex = highlightIndex;
    tft.fillScreen(TFT_BLACK);
    showPng(readPngIntoArray(images[imageIndex]), 0, 0);
}

void Gallery::closeImage(){
    imageIndex= -1;
    tft.fillScreen(TFT_BLACK);
    showThumbnails(images);
    drawHighlightBox();
}

void Gallery::goToNextHighlightBox(){
  highlightIndex++;
  Serial.printf("Highlighting index %d\n", highlightIndex);
 
  
  if(highlightIndex >= images.size()){
    Serial.println("Resetting highlight index");
    highlightIndex=0;
  }
}

void Gallery::drawHighlightBox(){
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

void Gallery::showPng(IMG_HOLDER imageHolder, uint16_t x, uint16_t y){
  unsigned long start = micros();
  tft.startWrite();
  tft.pushImage(x,y, imageHolder.width, imageHolder.height, imageHolder.imageBytes);
  tft.endWrite();
  Serial.println("Finished drawing");
  free(imageHolder.imageBytes);
  Serial.println("Freed buffer");
}

void Gallery::showThumbnails(std::vector<char*> thumbnailPaths){
  unsigned long start = micros();
  int thumbnailX=0, thumbnailY=0;
  for (size_t i = 0; i < thumbnailPaths.size(); i++){
    showThumbnail(i);
  }
  Serial.printf("Showing %d thumbnails\n", thumbnailPaths.size());

}

void Gallery::showThumbnail(int i){
  showThumbnail(images[i], getBoxX(i), getBoxY(i));
}

void Gallery::showThumbnail(char* tnPath, int x, int y){
    Serial.printf("Creating thumbnail for %s\n", tnPath);
    IMG_HOLDER imageHolder = readPngIntoArray(tnPath);
    IMG_HOLDER thumbnailHolder = createThumbnailNearest(&imageHolder);
    free(imageHolder.imageBytes);
    showPng(thumbnailHolder, x, y);
}

uint8_t Gallery::getBoxX(int i){
  return (i % GRID_MAX_COLS) * GRID_ELEMENT_WIDTH;
}

uint8_t Gallery::getBoxY(int i){
  return (i / GRID_MAX_COLS) * GRID_ELEMENT_HEIGHT;
}


