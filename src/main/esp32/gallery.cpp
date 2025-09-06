#include "gallery.h"
#include "image/image_factory.h"
#include "../core/configs.h"
#include <cstring>
#include "../core/img_holder.h"
#include "thumbnail_utils.h"
#include "log.h"

Gallery::~Gallery() {
}

void Gallery::init() {
    
    fs::File root = LittleFS.open("/", "r");
    while(fs::File file = root.openNextFile()){
        char buf[1000];
        snprintf(buf, sizeof(buf), "/%s", file.name());
        LOGF("Found file: %s", buf);
        Image* img = ImageFactory::createImage(buf);
        if(img){
            img->setPosition(0,0); // todo decide where to set position
            images.push_back(img);
            imageNames.push_back(strdup(buf));
        }
    }
    
    LOGF("Found %d images\n", images.size());  
    LOGF("Max cols: %d, max rows: %d\n", GRID_MAX_COLS, GRID_MAX_ROWS);

    showThumbnails(imageNames);
    drawHighlightBox();
}

void Gallery::draw(){
    if(imageIndex >= 0){
        images[imageIndex]->render(renderer);
    }
}

void Gallery::nextImage(){
    // todo move to some util
    imageIndex++;
    if(imageIndex >= images.size()){
        imageIndex = 0;
    }
    renderer.reset();
    images[imageIndex]->render(renderer);
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
    renderer.reset();
    images[imageIndex]->render(renderer);
  }

void Gallery::closeImage(){
    imageIndex= -1;
    tft.fillScreen(TFT_BLACK);
    showThumbnails(imageNames);
    drawHighlightBox();
}

void Gallery::goToNextHighlightBox(){
  highlightIndex++;
  LOGF("Highlighting index %d\n", highlightIndex);
 
  
  if(highlightIndex >= images.size()){
    LOG("Resetting highlight index");
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
  LOG("Finished drawing");
  free(imageHolder.imageBytes);
  LOG("Freed buffer");
}

void Gallery::showThumbnails(std::vector<char*> thumbnailPaths){
  unsigned long start = micros();
  int thumbnailX=0, thumbnailY=0;
  for (size_t i = 0; i < thumbnailPaths.size(); i++){
    showThumbnail(i);
  }
  LOGF("Showing %d thumbnails\n", thumbnailPaths.size());

}

void Gallery::showThumbnail(int i){
  showThumbnail(imageNames[i], getBoxX(i), getBoxY(i));
}

void Gallery::showThumbnail(char* tnPath, int x, int y){
    LOGF("Creating thumbnail for %s\n", tnPath);
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


