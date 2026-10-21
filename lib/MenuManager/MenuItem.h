#pragma once
#include <Arduino.h>
class MenuItem
{
public:
    using Action = void (*)();

    explicit MenuItem(const char* title, Action action = nullptr)
        : title_(title), action_(action) {}

    bool addChild(MenuItem& child)
    {
        if (&child == this || child.parent_)
            return false;

        // Ngăn tạo vòng lặp trong cây
        for (MenuItem* p = this; p; p = p->parent_)
        {
            if (p == &child)
                return false;
        }

        child.parent_ = this;

        if (!firstChild_)
        {
            firstChild_ = &child;
        }
        else
        {
            MenuItem* last = firstChild_;

            while (last->nextSibling_)
                last = last->nextSibling_;

            last->nextSibling_ = &child;
        }

        return true;
    }

    const char* title() const { return title_; }
    MenuItem* parent() const { return parent_; }
    MenuItem* firstChild() const { return firstChild_; }
    MenuItem* nextSibling() const { return nextSibling_; }

    MenuItem* childAt(uint16_t index) const
    {
        MenuItem* item = firstChild_;

        while (item && index--)
            item = item->nextSibling_;

        return item;
    }

    uint16_t childCount() const
    {
        uint16_t count = 0;

        for (MenuItem* p = firstChild_; p; p = p->nextSibling_)
            count++;

        return count;
    }

    void execute() const
    {
        if (action_)
            action_();
    }

private:
    const char* title_;
    Action action_;

    MenuItem* parent_ = nullptr;
    MenuItem* firstChild_ = nullptr;
    MenuItem* nextSibling_ = nullptr;
};