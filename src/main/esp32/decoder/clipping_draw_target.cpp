#include "clipping_draw_target.h"
#include <algorithm>
#include <Arduino.h>



ClippingDrawTarget::ClippingDrawTarget(IDrawTarget* delegate, int16_t clipW, int16_t clipH)
    : DelegatingDrawTarget(delegate), m_clipW(clipW), m_clipH(clipH) {
    size_t totalPixels = static_cast<size_t>(clipW)*clipH;
    // allocate PSRAM buffer once
    m_outBuffer = static_cast<uint16_t*>(ps_malloc(totalPixels * sizeof(uint16_t)));
}

ClippingDrawTarget::~ClippingDrawTarget() {
    free(m_outBuffer);
}

void ClippingDrawTarget::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    m_windowX = x;
    m_windowY = y;
    m_windowW = w;
    m_windowH = h;
}

void ClippingDrawTarget::pushPixels(const void* pixelsPtr, uint32_t count) {
    const uint16_t* src = static_cast<const uint16_t*>(pixelsPtr);

       // intersection of window and clip
    uint16_t ix0 = std::max<uint16_t>(m_windowX, 0);
    uint16_t iy0 = std::max<uint16_t>(m_windowY, 0);
    uint16_t ix1 = std::min<uint16_t>(m_windowX + m_windowW, m_clipW);
    uint16_t iy1 = std::min<uint16_t>(m_windowY + m_windowH + 1, m_clipH);

    if (ix0 >= ix1 || iy0 >= iy1) {
        // completely outside, nothing to draw
        return;
    }

    uint16_t drawW = ix1 - ix0;
    uint16_t drawH = iy1 - iy0;

    // copy row by row into clipped buffer
    for (uint16_t iy = 0; iy < drawH; ++iy) {
        uint16_t srcRow = (iy + iy0 - m_windowY) * m_windowW;
        uint16_t srcCol = ix0 - m_windowX;
        memcpy(
            &m_outBuffer[iy * drawW],
            &src[srcRow + srcCol],
            drawW * sizeof(uint16_t)
        );
    }

    // push clipped window to delegate
    delegate->setAddrWindow(ix0, iy0, drawW, drawH - 1); // -1 because height is offset
    delegate->pushPixels(m_outBuffer, drawW * drawH);
}
