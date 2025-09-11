#include "downscale_draw_target.h"
#include <cstring>

DownscaleDrawTarget::DownscaleDrawTarget(IDrawTarget *delegate, 
                                         uint16_t widthScale,
                                         uint16_t heightScale):
      DelegatingDrawTarget(std::move(delegate)),
      m_widthScale(widthScale),
      m_heightScale(heightScale),
      m_windowX(0), m_windowY(0), m_windowW(0), m_windowH(0),
      m_cursorX(0), m_cursorY(0)
{
}

void DownscaleDrawTarget::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    m_windowX = x;
    m_windowY = y;
    m_windowW = w;
    m_windowH = h;
    m_cursorX = 0;
    m_cursorY = 0;

    // Downscaled window
    uint16_t dw = w / m_widthScale;
    uint16_t dh = h / m_heightScale;

    delegate->setAddrWindow(x / m_widthScale, y / m_heightScale, dw, dh);

    // Allocate buffer for one chunk of downscaled pixels
    m_downBuf.clear();
    m_downBuf.reserve(dw * dh);
}

void DownscaleDrawTarget::pushPixels(const void *pixels, uint32_t count) {
    const uint16_t* src = static_cast<const uint16_t*>(pixels);

    for (uint32_t i = 0; i < count; ++i) {
        // Absolute source position
        uint16_t absX = m_windowX + m_cursorX;
        uint16_t absY = m_windowY + m_cursorY;

        if ((absX % m_widthScale == 0) && (absY % m_heightScale == 0)) {
            m_downBuf.push_back(src[i]);
        }

        // Move cursor
        ++m_cursorX;
        if (m_cursorX >= m_windowW) {
            m_cursorX = 0;
            ++m_cursorY;
        }
    }

    // Flush if we collected enough downscaled pixels for a row
    if (!m_downBuf.empty()) {
        delegate->pushPixels(m_downBuf.data(), m_downBuf.size());
        m_downBuf.clear();
    }
}
