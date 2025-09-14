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

void OverlayDrawTarget::pushPixels(const void *pixels, uint32_t count) {

    const uint16_t* src = static_cast<const uint16_t*>(pixels);

    uint32_t i = 0;
    for (uint16_t row = 0; row <= m_windowH; ++row) {
        for (uint16_t col = 0; col < m_windowW; ++col) {
            uint16_t absX = m_windowX + col;
            uint16_t absY = m_windowY + row;

            bool inOverlay =
                absX >= m_overlayX &&
                absX < (m_overlayX + m_overlayWidth) &&
                absY >= m_overlayY &&
                absY < (m_overlayY + m_overlayHeight);

            if (inOverlay) {
                LOGF("In overlay %d, %d", absX, absY);
                uint16_t ox = absX - m_overlayX;
                uint16_t oy = absY - m_overlayY;
                m_outBuffer[i] = m_overlayPixels[oy * m_overlayWidth + ox];
            } else {
                m_outBuffer[i] = src[i];
            }

            ++i;
            if (i >= count) break; // safety
        }
        if (i >= count) break;
    }

    delegate->pushPixels(m_outBuffer, count);
}
