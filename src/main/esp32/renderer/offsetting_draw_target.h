#ifndef __ESP_GALLERY_OFFSETTING_DRAW_TARGET_H__
#define __ESP_GALLERY_OFFSETTING_DRAW_TARGET_H__
#include "../decoder/delegating_draw_target.h"


class OffsettingDrawTarget : public DelegatingDrawTarget{

    public:
        OffsettingDrawTarget(IDrawTarget *delegate, int16_t x, int16_t y): DelegatingDrawTarget(delegate){
            xOffset = x;
            yOffset = y;
        }

        void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) override;
        void pushPixels(const void *pixels, uint32_t count) override;

    private:
        int16_t xOffset,yOffset;
};


#endif