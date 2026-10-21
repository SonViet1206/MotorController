#pragma once
#include "IMenuRenderer.h"

template <typename TLcd>
class LCDRenderer : public IMenuRenderer
{
public:
    LCDRenderer(TLcd& lcd, uint8_t cols, uint8_t rows)
        : lcd_(lcd), cols_(cols), rows_(rows) {}

    void begin() override
    {
        // LCD phải được khởi tạo trước từ setup().
        lcd_.clear();
    }

    uint8_t visibleRows() const override
    {
        return rows_;
    }

    void startFrame(const char* title) override
    {
        // Với LCD ký tự, ưu tiên toàn bộ dòng cho menu.
        // Title hiện chưa được hiển thị.
        (void)title;
    }

    void drawItem(
        uint8_t row,
        const char* text,
        bool selected,
        bool hasChildren
    ) override
    {
        if (row >= rows_) return;

        lcd_.setCursor(0, row);

        lcd_.print(selected ? '>' : ' ');

        uint8_t width = cols_ > 2 ? cols_ - 2 : 0;
        uint8_t i = 0;

        while (i < width && text[i] != '\0')
        {
            lcd_.print(text[i]);
            i++;
        }

        while (i < width)
        {
            lcd_.print(' ');
            i++;
        }

        if (cols_ >= 2)
            lcd_.print(hasChildren ? '>' : ' ');
    }

    void endFrame() override {}

private:
    TLcd& lcd_;
    uint8_t cols_;
    uint8_t rows_;
};