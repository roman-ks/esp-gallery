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
        Image *img = ImageFactory::createImage(buf);
        if(img){
            img->setPosition(0,0); // todo decide where to set position
            images.push_back(img);
        }

        Image *thumbnail = ImageFactory::createDownscaledImage(buf);
        if(thumbnail){
          thumbnails.push_back(thumbnail);
        }

    }
    
    LOGF("Found %d images\n", images.size());  
    LOGF("Max cols: %d, max rows: %d\n", GRID_MAX_COLS, GRID_MAX_ROWS);

    showThumbnails(thumbnails);
    drawHighlightBox(HIGHLIGHT_COLOR);
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
    // overwrite current highlight
    drawHighlightBox(BG_COLOR);   
    showThumbnail(highlightIndex);

    goToNextHighlightBox();
    drawHighlightBox(HIGHLIGHT_COLOR);  
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
    renderer.reset();
    showThumbnails(thumbnails);
    drawHighlightBox(HIGHLIGHT_COLOR);
}

void Gallery::goToNextHighlightBox(){
  highlightIndex++;
  LOGF("Highlighting index %d\n", highlightIndex);
 
  
  if(highlightIndex >= images.size()){
    LOG("Resetting highlight index");
    highlightIndex=0;
  }
}

void Gallery::drawHighlightBox(uint32_t color){
  renderer.renderBorder(getBoxX(highlightIndex), getBoxY(highlightIndex), 
    GRID_ELEMENT_WIDTH, GRID_ELEMENT_HEIGHT, HIGHLIGHT_THICKNESS, color);
}

void Gallery::showThumbnails(std::vector<Image*> thumbnails){
  for (size_t i = 0; i < thumbnails.size(); i++){
      showThumbnail(i);
  }
  LOGF("Showing %d thumbnails\n", thumbnails.size());
}

void Gallery::showThumbnail(int i){
    showThumbnail(thumbnails[i], getBoxX(i), getBoxY(i));
}

void Gallery::showThumbnail(Image* thumbnail, uint8_t x, uint8_t y){
    LOGF("Rendering thumbnail %s\n", thumbnail->filePath);
    thumbnail->setPosition(x, y);
    thumbnail->render(renderer);
}

uint16_t Gallery::getBoxX(int i){
  return (i % GRID_MAX_COLS) * GRID_ELEMENT_WIDTH;
}

uint16_t Gallery::getBoxY(int i){
  return (i / GRID_MAX_COLS) * GRID_ELEMENT_HEIGHT;
}


