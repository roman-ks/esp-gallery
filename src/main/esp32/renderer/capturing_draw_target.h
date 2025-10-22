#pragma once

#include "../decoder/delegating_draw_target.h"
#include <cstdint>
#include "pixels_holder.h"
#include <memory>

class CapturingDrawTarget : public DelegatingDrawTarget {
public:
    CapturingDrawTarget(IDrawTarget* delegate);
    ~CapturingDrawTarget();

    virtual void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) override;
    virtual void pushPixels(const void *pixels, uint32_t count) override;

    std::unique_ptr<PixelsHolder> getCaptured();

private:
    uint16_t m_width;
    uint16_t m_height;

    uint16_t* m_pixels;      // PSRAM buffer for whole image
    size_t maxPixels;

    uint16_t m_windowX;
    uint16_t m_windowY;
    uint16_t m_windowW;
    uint16_t m_windowH;

    uint16_t m_cursorX;
    uint16_t m_cursorY;
};
