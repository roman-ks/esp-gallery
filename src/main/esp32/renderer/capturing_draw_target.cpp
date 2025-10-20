#include "capturing_draw_target.h"
#include <Arduino.h>
#include <cstring>
#include "../../core/configs.h"
#define LOG_LEVEL 1
#include "../log.h"

CapturingDrawTarget::CapturingDrawTarget(IDrawTarget* delegate): DelegatingDrawTarget(delegate){
    m_width = m_height = m_windowX = m_windowY =m_windowW = m_windowH = m_cursorX = m_cursorY = 0;

    size_t totalPixels = static_cast<size_t>(MAX_IMAGE_WIDTH) * MAX_IMAGE_HEIGHT;

    m_pixels = static_cast<uint16_t*>(ps_malloc(totalPixels * sizeof(uint16_t)));

    if (m_pixels) std::memset(m_pixels, 0, totalPixels * sizeof(uint16_t));
}

void CapturingDrawTarget::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    if (x + w > m_width) m_width = x + w;
    if (y + h > m_height) m_height = y + h;
    LOGF_T("y=%d, h=%d, m_height=%d\n", y,h, m_height);

    m_windowX = x;
    m_windowY = y;
    m_windowW = w;
    m_windowH = h;

    m_cursorX = 0;
    m_cursorY = 0;
    delegate->setAddrWindow(x,y,w,h);
}

void CapturingDrawTarget::pushPixels(const void *pixels, uint32_t count) {
    const uint16_t* src = static_cast<const uint16_t*>(pixels);
    size_t i = 0; // index in src
    // Fill combined buffer for the current window
    for (uint16_t row = 0; row <= m_windowH; ++row) {
        for (uint16_t col = 0; col < m_windowW; ++col) {
            uint16_t absX = m_windowX + col;
            uint16_t absY = m_windowY + row;
            size_t index = absY * m_width + absX;

            m_pixels[index] = src[i];
            i++;
        }
    }

    m_cursorX = 0;
    m_cursorY = m_windowH;

    // Push input to delegate
    delegate->pushPixels(pixels, count);
}

std::unique_ptr<PixelsHolder> CapturingDrawTarget::getCaptured(){
    return std::make_unique<PixelsHolder>(m_width, m_height, m_pixels);
}
