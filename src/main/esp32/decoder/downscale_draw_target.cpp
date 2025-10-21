#include "downscale_draw_target.h"
#include <cstring>
#include "../../core/configs.h"

DownscaleDrawTarget::DownscaleDrawTarget(IDrawTarget *delegate, 
                                         uint16_t widthScale,
                                         uint16_t heightScale):
      DelegatingDrawTarget(std::move(delegate)),
      m_widthScale(widthScale),
      m_heightScale(heightScale),
      m_windowX(0), m_windowY(0), m_windowW(0), m_windowH(0),
      m_cursorX(0), m_cursorY(0),
      m_downBufCount(0)

{
    m_downBuf = std::make_unique<uint16_t[]>(MAX_IMAGE_WIDTH/ widthScale * 
                                             MAX_IMAGE_HEIGHT/ heightScale);
}

void DownscaleDrawTarget::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    m_windowX = x;
    m_windowY = y;
    m_windowW = w;
    m_windowH = h;
    m_cursorX = 0;
    m_cursorY = 0;

    // Downscaled window (use ceiling division so we pick the correct number
    // of samples when window dimensions are not exact multiples of the
    // scale factors). For example, a 16-wide window with scale 3 should
    // produce 6 pixels: indices 0,3,6,9,12,15.
    uint16_t dw = (w + m_widthScale - 1) / m_widthScale;
    uint16_t dh = (h + m_heightScale - 1) / m_heightScale;

    delegate->setAddrWindow(x / m_widthScale, y / m_heightScale, dw, dh);

    // Reset fixed buffer
    m_downBufCount = 0;
}

void DownscaleDrawTarget::pushPixels(const void *pixels, uint32_t count) {
    const uint16_t* src = static_cast<const uint16_t*>(pixels);

    for (uint32_t i = 0; i < count; ++i) {
        // Use window-relative cursor position so downscaling blocks align to the
        // window origin rather than the absolute framebuffer origin. Using
        // absolute coordinates caused a grid artifact when window offsets
        // weren't multiples of the scale factors.
        if ((m_cursorX % m_widthScale == 0) && (m_cursorY % m_heightScale == 0)) {
            m_downBuf[m_downBufCount++] = src[i];
        }

        // Move cursor
        ++m_cursorX;
        if (m_cursorX >= m_windowW) {
            m_cursorX = 0;
            ++m_cursorY;
        }
    }

    // Flush if we collected enough downscaled pixels for a row
    if (m_downBufCount > 0) {
        delegate->pushPixels(m_downBuf.get(), static_cast<uint32_t>(m_downBufCount));
        m_downBufCount = 0;
    }
}
