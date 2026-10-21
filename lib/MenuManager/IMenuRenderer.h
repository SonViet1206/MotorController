#pragma once
#include <Arduino.h>

class IMenuRenderer
{
public:
    virtual ~IMenuRenderer() = default;

    virtual void begin() = 0;
    virtual uint8_t visibleRows() const = 0;

    virtual void startFrame(const char* title) = 0;

    virtual void drawItem(
        uint8_t row,
        const char* text,
        bool selected,
        bool hasChildren
    ) = 0;

    virtual void endFrame() = 0;
};