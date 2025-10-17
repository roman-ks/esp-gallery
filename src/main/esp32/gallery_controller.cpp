#include "gallery_controller.h"


GalleryController::~GalleryController() {
}

void GalleryController::loop(){
    gallery.draw();
}

void GalleryController::handleRightButtonPress(){
    if(gallery.isImageOpen()){
        gallery.nextImage();
    } else {
        gallery.nextHighlight();
    }
}

void GalleryController::handleLeftButtonPress(){
    if(gallery.isImageOpen()){
        gallery.prevImage();
    } else {
        gallery.prevHighlight();
    }
}

void GalleryController::handleRightButtonLongPress(){
    if(!gallery.isImageOpen()){
        gallery.nextPage();
    }
}

void GalleryController::handleLeftButtonLongPress(){
    if(!gallery.isImageOpen()){
        gallery.prevPage();
    }
}

void GalleryController::handleEnterButtonPress(){
    if(gallery.isImageOpen()){
        gallery.closeImage();
    } else {
        gallery.openImage();
    }
}

