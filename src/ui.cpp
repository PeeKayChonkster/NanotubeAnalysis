#include "ui.hpp"
#include <algorithm>
#include "prim_exception.hpp"

//--- Control ---//

nano::Control::Control(std::string name): Control(name, {0,0,0,0}) {}

nano::Control::Control(std::string name, raylib::Rectangle boundingRect, raylib::Color backgroundColor) : name(name), boundingRect(boundingRect), backgroundColor(backgroundColor) {}

void nano::Control::drawChildren()
{
    for(Control* child : children)
    {
        child->draw();
    }
}

void nano::Control::addChild(Control* child)
{
    if(std::find(children.begin(), children.end(), child) == children.end())
    {
        children.push_back(child);
        if(child->parent) child->parent->removeChild(child);
        child->parent = this;
    }
    else
    {
        throw PRIM_EXCEPTION("Trying to add same ui child twice.");
    }
}

void nano::Control::removeChild(Control* child)
{
    auto iter = std::find(children.begin(), children.end(), child);
    if(iter != children.end())
    {
        children.erase(iter);
        child->parent = nullptr;
    }
    else
    {
        throw PRIM_EXCEPTION("Trying to remove non-existent child.");
    }
}


std::string nano::Control::getName() const { return name; }

raylib::Vector2 nano::Control::getPosition() const { return { boundingRect.x, boundingRect.y }; }

void nano::Control::setPosition(raylib::Vector2 position) { boundingRect.SetPosition(position); }

raylib::Vector2 nano::Control::getSize() const { return { boundingRect.width, boundingRect.height }; }

void nano::Control::setSize(raylib::Vector2 size) { boundingRect.SetSize(size); }

int nano::Control::getWidth() const { return boundingRect.width; }

int nano::Control::getHeight() const { return boundingRect.height; }

void nano::Control::setWidth(uint16_t width) { boundingRect.SetWidth(width); }

void nano::Control::setHeight(uint16_t width) { boundingRect.SetHeight(width); }

raylib::Color nano::Control::getBackgroundColor() const { return backgroundColor; }

void nano::Control::setBackgroundColor(raylib::Color color) { backgroundColor = color; }

void nano::Control::centralize()
{
    if(!parent) return;
    setPosition({ (parent->getWidth() - getWidth()) * 0.5f, (parent->getHeight() - getHeight()) * 0.5f });
}

void nano::Control::centralize(const Control* box)
{
    setPosition({ (box->getWidth() - getWidth()) * 0.5f, (box->getHeight() - getHeight()) * 0.5f });
}

void nano::Control::centralize(const raylib::Window* window)
{
    setPosition({ (window->GetWidth() - getWidth()) * 0.5f, (window->GetHeight() - getHeight()) * 0.5f });
}

void nano::Control::draw()
{
    UI_DRAW_OVERRIDE
    
    const raylib::Vector2& parentPos = parent? parent->getPosition() : raylib::Vector2::Zero();
    ::DrawRectangle(parentPos.x + boundingRect.x, parentPos.y + boundingRect.y, getWidth(), getHeight(), backgroundColor);
}



//--- Label ---//

nano::Label::Label(std::string name) : Label(name, "") {}

nano::Label::Label(std::string name, std::string text) : Control(name), text(text) {}

nano::Label::Label(std::string name, raylib::Rectangle boundingRect, raylib::Color backgroundColor, std::string text) : Control(name, boundingRect, backgroundColor), text(text) {}

std::string_view nano::Label::getText() const { return text; }

void nano::Label::setText(std::string text) { this->text = text; }

void nano::Label::draw()
{
    UI_DRAW_OVERRIDE

    const raylib::Vector2& parentPos = parent? parent->getPosition() : raylib::Vector2::Zero();
    switch(sizePolicy)
    {
        case SizePolicy::FIXED:
            break;
        case SizePolicy::EXPAND:
            {
                raylib::Vector2 textSize = ::MeasureTextEx(::GetFontDefault(), text.c_str(), fontSize, 1);
                if(textSize.x > boundingRect.width) boundingRect.width = textSize.x;
                if(textSize.y > boundingRect.height) boundingRect.height = textSize.y;
                break;
            }
        case SizePolicy::SHRINK:
            {
                raylib::Vector2 textSize = ::MeasureTextEx(::GetFontDefault(), text.c_str(), fontSize, 1);
                setSize(textSize);
                break;
            }
    }
    raylib::Rectangle labelBounds = boundingRect;
    ::DrawRectangle(parentPos.x + boundingRect.x, parentPos.y + boundingRect.y, getWidth(), getHeight(), backgroundColor);
    ::GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    ::GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, textColor);
    ::GuiLabel(labelBounds, text.c_str());
}