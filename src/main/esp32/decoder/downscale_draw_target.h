#pragma once

#include "delegating_draw_target.h"
#include <cstdint>
#include <vector>

class DownscaleDrawTarget : public DelegatingDrawTarget {
public:
    DownscaleDrawTarget(IDrawTarget *delegate, uint16_t widthScale,
                        uint16_t heightScale);

    virtual void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) override;
    virtual void pushPixels(const void *pixels, uint32_t count) override;

private:
    uint16_t m_widthScale;
    uint16_t m_heightScale;

    // Current window in "source" coordinates
    uint16_t m_windowX;
    uint16_t m_windowY;
    uint16_t m_windowW;
    uint16_t m_windowH;

    uint16_t m_cursorX;
    uint16_t m_cursorY;

    // Buffer for downscaled pixels before delegation
    std::vector<uint16_t> m_downBuf;
};
