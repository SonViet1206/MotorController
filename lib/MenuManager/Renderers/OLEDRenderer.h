#pragma once
#include <U8g2lib.h>
#include "IMenuRenderer.h"

class OLEDRenderer : public IMenuRenderer
{
public:
    OLEDRenderer(U8G2& display, uint8_t lineHeight = 12)
        : display_(display), lineHeight_(lineHeight) {}

    void begin() override
    {
        display_.begin();
        display_.setFont(u8g2_font_6x10_tf);
    }

    uint8_t visibleRows() const override
    {
        uint8_t h = display_.getDisplayHeight();

        return h > lineHeight_
            ? (h - lineHeight_) / lineHeight_
            : 0;
    }

    void startFrame(const char* title) override
    {
        display_.clearBuffer();

        display_.setFont(u8g2_font_6x10_tf);

        display_.drawStr(0, 10, title);
        display_.drawHLine(
            0, lineHeight_ - 1,
            display_.getDisplayWidth()
        );
    }

    void drawItem(
        uint8_t row,
        const char* text,
        bool selected,
        bool hasChildren
    ) override
    {
        uint8_t y = lineHeight_ * (row + 2) - 2;
        uint8_t top = lineHeight_ * (row + 1);

        if (selected)
        {
            display_.drawBox(
                0, top,
                display_.getDisplayWidth(),
                lineHeight_
            );

            display_.setDrawColor(0);
        }
        else
        {
            display_.setDrawColor(1);
        }

        display_.drawStr(2, y, text);

        if (hasChildren)
        {
            uint8_t x = display_.getDisplayWidth() - 8;
            display_.drawStr(x, y, ">");
        }

        display_.setDrawColor(1);
    }

    void endFrame() override
    {
        display_.sendBuffer();
    }

private:
    U8G2& display_;
    uint8_t lineHeight_;
};