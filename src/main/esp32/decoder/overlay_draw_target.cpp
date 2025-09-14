#include "overlay_draw_target.h"
#include "../../core/configs.h"
#include <stdlib.h>
#include <Arduino.h>
#include "../log.h"

OverlayDrawTarget::OverlayDrawTarget(IDrawTarget* delegate,
                                     const uint16_t* overlayPixels,
                                     uint16_t overlayWidth,
                                     uint16_t overlayHeight,
                                     uint16_t overlayX,
                                     uint16_t overlayY)
    : DelegatingDrawTarget(delegate),
      m_overlayPixels(overlayPixels),
      m_overlayWidth(overlayWidth),
      m_overlayHeight(overlayHeight),
      m_overlayX(overlayX),
      m_overlayY(overlayY),
      m_windowX(0),
      m_windowY(0),
      m_windowW(0),
      m_windowH(0){
    size_t totalPixels = static_cast<size_t>(MAX_IMAGE_WIDTH) * MAX_IMAGE_HEIGHT;
    // allocate PSRAM buffer once
    m_outBuffer = static_cast<uint16_t*>(ps_malloc(totalPixels * sizeof(uint16_t)));
}

OverlayDrawTarget::~OverlayDrawTarget() {
    free(m_outBuffer);
}

void OverlayDrawTarget::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    m_windowX = x;
    m_windowY = y;
    m_windowW = w;
    m_windowH = h;

    delegate->setAddrWindow(x, y, w, h);
}

void OverlayDrawTarget::pushPixels(const void *pixelsPtr, uint32_t count) {
        const uint16_t* src = static_cast<const uint16_t*>(pixelsPtr);

        // compute intersection rectangle
        int ix0 = std::max<int>(m_windowX, m_overlayX);
        int iy0 = std::max<int>(m_windowY, m_overlayY);
        int ix1 = std::min<int>(m_windowX + m_windowW, m_overlayX + m_overlayWidth);
        // m_windowH is offset, +1 to convert to count
        int iy1 = std::min<int>(m_windowY + m_windowH + 1, m_overlayY + m_overlayHeight);

        if (ix0 >= ix1 || iy0 >= iy1) {
            // no overlap, delegate directly
            delegate->pushPixels(pixelsPtr, count);
            return;
        }

        // make a copy of the window pixels
        for (uint32_t i = 0; i < count; ++i)
            memcpy(m_outBuffer, src, count);

        // overlay only on intersection area
        for (int iy = iy0; iy < iy1; ++iy) {
            for (int ix = ix0; ix < ix1; ++ix) {
                int overlayIdx = (iy - m_overlayY) * m_overlayWidth + (ix - m_overlayX);
                int bufferIdx  = (iy - m_windowY) * m_windowW + (ix - m_windowX);
                m_outBuffer[bufferIdx] = m_overlayPixels[overlayIdx];
            }
        }

        delegate->pushPixels(m_outBuffer, count);
    }
