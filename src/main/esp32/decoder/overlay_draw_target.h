#pragma once
#include "delegating_draw_target.h"
#include <cstdint>

class OverlayDrawTarget : public DelegatingDrawTarget {
    public:
        OverlayDrawTarget(IDrawTarget* delegate,
                        const uint16_t* overlayPixels,
                        uint16_t overlayWidth,
                        uint16_t overlayHeight,
                        uint16_t overlayX,
                        uint16_t overlayY);
        ~OverlayDrawTarget();


        virtual void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) override;
        virtual void pushPixels(const void *pixels, uint32_t count) override;

    private:
        const uint16_t* m_overlayPixels;
        uint16_t m_overlayWidth;
        uint16_t m_overlayHeight;
        uint16_t m_overlayX;
        uint16_t m_overlayY;

        // current drawing window
        uint16_t m_windowX;
        uint16_t m_windowY;
        uint16_t m_windowW;
        uint16_t m_windowH;

        // psram-backed buffer
        uint16_t* m_outBuffer;
        size_t    m_outCapacity;
};
