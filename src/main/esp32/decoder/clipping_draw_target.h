#pragma once
#include "delegating_draw_target.h"
#include <cstdint>
#include <algorithm>

class ClippingDrawTarget : public DelegatingDrawTarget {
public:
    ClippingDrawTarget(IDrawTarget* delegate, int16_t clipW, int16_t clipH);
    ~ClippingDrawTarget();

    void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) override;
    void pushPixels(const void* pixelsPtr, uint32_t count) override;

private:
    uint16_t m_clipW, m_clipH;
    uint16_t m_windowX = 0, m_windowY = 0, m_windowW = 0, m_windowH = 0;

    uint16_t *m_outBuffer = nullptr;
};
