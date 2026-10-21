#pragma once
#include "MenuItem.h"
#include "IMenuRenderer.h"

class MenuManager
{
public:
    MenuManager(MenuItem& root, IMenuRenderer& renderer)
        : root_(root), current_(&root), renderer_(renderer) {}

    void begin()
    {
        renderer_.begin();
        render();
    }

    void next()
    {
        uint16_t count = current_->childCount();
        if (!count) return;

        selected_ = (selected_ + 1) % count;
        render();
    }

    void previous()
    {
        uint16_t count = current_->childCount();
        if (!count) return;

        selected_ = (selected_ + count - 1) % count;
        render();
    }

    void enter()
    {
        MenuItem* item = current_->childAt(selected_);
        if (!item) return;

        if (item->firstChild())
        {
            current_ = item;
            selected_ = 0;
            firstVisible_ = 0;
            render();
        }
        else
        {
            item->execute();
            render();
        }
    }

    void back()
    {
        if (current_ == &root_) return;

        MenuItem* old = current_;
        current_ = current_->parent();

        selected_ = 0;
        uint16_t index = 0;

        for (MenuItem* p = current_->firstChild();
             p;
             p = p->nextSibling(), index++)
        {
            if (p == old)
            {
                selected_ = index;
                break;
            }
        }

        firstVisible_ = 0;
        render();
    }

    void render()
    {
        uint8_t rows = renderer_.visibleRows();
        if (!rows) return;

        if (selected_ < firstVisible_)
            firstVisible_ = selected_;

        if (selected_ >= (uint16_t)firstVisible_ + rows)
            firstVisible_ = selected_ - rows + 1;

        renderer_.startFrame(current_->title());

        for (uint8_t row = 0; row < rows; row++)
        {
            uint16_t index = firstVisible_ + row;
            MenuItem* item = current_->childAt(index);

            if (item)
            {
                renderer_.drawItem(
                    row,
                    item->title(),
                    index == selected_,
                    item->firstChild() != nullptr
                );
            }
            else
            {
                renderer_.drawItem(row, "", false, false);
            }
        }

        renderer_.endFrame();
    }

private:
    MenuItem& root_;
    MenuItem* current_;
    IMenuRenderer& renderer_;

    uint16_t selected_ = 0;
    uint16_t firstVisible_ = 0;
};