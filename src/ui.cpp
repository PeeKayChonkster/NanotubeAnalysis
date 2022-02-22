#include "ui.hpp"
#include <algorithm>
#include "prim_exception.hpp"
#include <iostream>

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
    {   // if didn't found child in direct children, try to find it in children of children
        for(auto& ch : children)
        {
            ch->removeChild(child);
        }
    }
}

void nano::Control::destroy()
{
    if(parent) parent->removeChild(this);

    for(int i = 0; i < children.size(); ++i)
    {
        children[i]->destroy();
    }

    std::cout << "Destroyed ui element: " << name << std::endl;
    delete this;
}

std::string nano::Control::getName() const { return name; }

raylib::Vector2 nano::Control::getPosition() const { return raylib::Vector2(boundingRect.x, boundingRect.y); }

raylib::Vector2 nano::Control::getGlobalPosition() const 
    { 
        raylib::Vector2 pos = raylib::Vector2(boundingRect.x, boundingRect.y);
        if(parent) pos += parent->getGlobalPosition();
        return std::move(pos);
    }

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

void nano::Control::draw()
{
    if(!visible) return;    
    ::DrawRectangleRec(raylib::Rectangle(getGlobalPosition(), getSize()),  backgroundColor);
    UI_DRAW_OVERRIDE
}



//--- Label ---//

nano::Label::Label(std::string name) : Label(name, "") {}

nano::Label::Label(std::string name, std::string text) : Label(name, text, WHITE, defaultFontSize) {}

nano::Label::Label(std::string name, raylib::Rectangle boundingRect, raylib::Color backgroundColor, std::string text) : Control(name, boundingRect, backgroundColor), text(text) {}

nano::Label::Label(std::string name, std::string text, raylib::Color textColor, uint8_t fontSize): Control(name), text(text), textColor(textColor), fontSize(fontSize) {}

std::string_view nano::Label::getText() const { return text; }

void nano::Label::setText(std::string text) { this->text = text; }

void nano::Label::draw()
{
    if(!visible) return;

    boundingRect.SetSize(::MeasureTextEx(::GetFontDefault(), text.c_str(), fontSize, 1));
    ::DrawRectangleRec(raylib::Rectangle(getGlobalPosition(), getSize()),  backgroundColor);
    ::GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    ::GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, textColor);
    ::GuiFade(textColor.a / 255.0f);
    ::GuiLabel(boundingRect, text.c_str());

    UI_DRAW_OVERRIDE
}


//--- Button ---//

nano::Button::Button(std::string name) : Label(name, "") {}

nano::Button::Button(std::string name, std::string text) : Button(name, raylib::Rectangle{ 5, 5, 100, 30 }, GREEN, text) {}

nano::Button::Button(std::string name, raylib::Rectangle boundingRect, raylib::Color backgroundColor, std::string text) : Label(name, boundingRect, backgroundColor, text) {}

void nano::Button::draw()
{
    if(!visible) return;

    if(shrink) setSize(::MeasureTextEx(::GetFontDefault(), text.c_str(), fontSize, 1));
    raylib::Rectangle destRec(getGlobalPosition(), getSize());
    ::GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    ::GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, textColor);
    ::GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, backgroundColor);
    pressed = ::GuiButton(destRec, text.c_str());
    if(callback && pressed) callback();
    
    
    UI_DRAW_OVERRIDE
}


//--- WindowBox ---//

nano::WindowBox::WindowBox(std::string name) : Control(name) {}

nano::WindowBox::WindowBox(std::string name, std::string title) : WindowBox(name, raylib::Rectangle{ 5, 5, 100, 30 }, GREEN, title) {}

nano::WindowBox::WindowBox(std::string name, raylib::Rectangle boundingRect, raylib::Color backgroundColor, std::string title) : Control(name, boundingRect, backgroundColor), title(title) {}

void nano::WindowBox::draw()
{
    if(!visible) return;

    ::GuiSetStyle(STATUSBAR, TEXT_COLOR_NORMAL, raylib::Color::White());
    ::GuiSetStyle(STATUSBAR, BASE_COLOR_NORMAL, backgroundColor);
    ::GuiSetStyle(DEFAULT, TEXT_SIZE, defaultFontSize);

    if(::GuiWindowBox(boundingRect, title.c_str()))
    {
        destroy();
    }

    UI_DRAW_OVERRIDE
}