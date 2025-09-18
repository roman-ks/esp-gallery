#include "offsetting_draw_target.h"

void OffsettingDrawTarget::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    delegate->setAddrWindow(x+xOffset, y+yOffset, w, h);
}

void OffsettingDrawTarget::pushPixels(const void *pixels, uint32_t count) {
    delegate->pushPixels(pixels, count);
}
