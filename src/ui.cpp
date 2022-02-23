#include "ui.hpp"
#include "prim_exception.hpp"
#include "debug.hpp"
#include <algorithm>
#include <iostream>

//--- Control ---//

nano::Control::Control(std::string name): Control(name, {0,0,0,0}) {}

nano::Control::Control(std::string name, raylib::Rectangle bounds, raylib::Color backgroundColor) : name(name), bounds(bounds), backgroundColor(backgroundColor) {}

void nano::Control::drawChildren()
{
    for(int i = 0; i < children.size(); ++i)
    {
        children[i]->draw();
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

raylib::Vector2 nano::Control::getPosition() const { return raylib::Vector2(bounds.x, bounds.y); }

raylib::Vector2 nano::Control::getGlobalPosition() const 
    { 
        raylib::Vector2 pos = raylib::Vector2(bounds.x, bounds.y);
        if(parent) pos += parent->getGlobalPosition();
        return std::move(pos);
    }

void nano::Control::setPosition(raylib::Vector2 position) { bounds.SetPosition(position); }

raylib::Vector2 nano::Control::getSize() const { return { bounds.width, bounds.height }; }

void nano::Control::setSize(raylib::Vector2 size) { bounds.SetSize(size); }

int nano::Control::getWidth() const { return bounds.width; }

int nano::Control::getHeight() const { return bounds.height; }

void nano::Control::setWidth(uint16_t width) { bounds.SetWidth(width); }

void nano::Control::setHeight(uint16_t width) { bounds.SetHeight(width); }

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
    ::GuiLoadStyleDefault();
    ::DrawRectangleRec(raylib::Rectangle(getGlobalPosition(), getSize()),  backgroundColor);
    UI_DRAW_OVERRIDE
}



//--- Label ---//

nano::Label::Label(std::string name) : Label(name, "") {}

nano::Label::Label(std::string name, std::string text) : Label(name, text, WHITE, defaultFontSize) {}

nano::Label::Label(std::string name, raylib::Rectangle bounds, raylib::Color backgroundColor, std::string text) : Control(name, bounds, backgroundColor), text(text) {}

nano::Label::Label(std::string name, std::string text, raylib::Color textColor, uint8_t fontSize): Control(name), text(text), textColor(textColor), fontSize(fontSize) {}

std::string_view nano::Label::getText() const { return text; }

void nano::Label::setText(std::string text) { this->text = text; }

void nano::Label::draw()
{
    if(!visible) return;

    bounds.SetSize(::MeasureTextEx(::GetFontDefault(), text.c_str(), fontSize, 1));
    ::GuiLoadStyleDefault();
    ::DrawRectangleRec(raylib::Rectangle(getGlobalPosition(), getSize()),  backgroundColor);
    ::GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    ::GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, textColor);
    ::GuiFade(textColor.a / 255.0f);
    ::GuiLabel(bounds, text.c_str());

    UI_DRAW_OVERRIDE
}


//--- Button ---//

nano::Button::Button(std::string name) : Label(name, "") {}

nano::Button::Button(std::string name, std::string text) : Button(name, raylib::Rectangle{ 5, 5, 100, 30 }, GREEN, text) {}

nano::Button::Button(std::string name, raylib::Rectangle bounds, raylib::Color backgroundColor, std::string text) : Label(name, bounds, backgroundColor, text) {}

void nano::Button::draw()
{
    if(!visible) return;

    if(shrink) setSize(::MeasureTextEx(::GetFontDefault(), text.c_str(), fontSize, 1));
    raylib::Rectangle dest(getGlobalPosition(), getSize());
    ::GuiLoadStyleDefault();
    ::GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    ::GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, textColor);
    ::GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, backgroundColor);
    pressed = ::GuiButton(dest, text.c_str());
    if(callback && pressed) callback();
    
    
    UI_DRAW_OVERRIDE
}


//--- WindowBox ---//

nano::WindowBox::WindowBox(std::string name) : Control(name) {}

nano::WindowBox::WindowBox(std::string name, std::string title) : WindowBox(name, raylib::Rectangle{ 5, 5, 100, 30 }, GREEN, title) {}

nano::WindowBox::WindowBox(std::string name, raylib::Rectangle bounds, raylib::Color backgroundColor, std::string title) : Control(name, bounds, backgroundColor), title(title) {}

void nano::WindowBox::draw()
{
    if(!visible) return;

    ::GuiLoadStyleDefault();
    ::GuiSetStyle(STATUSBAR, TEXT_COLOR_NORMAL, raylib::Color::White());
    ::GuiSetStyle(STATUSBAR, BASE_COLOR_NORMAL, backgroundColor);
    ::GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, raylib::Color::Gray());
    ::GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, raylib::Color::White());
    ::GuiSetStyle(DEFAULT, TEXT_SIZE, defaultFontSize);

    
    if(::IsMouseButtonPressed(0))
    {  
        raylib::Vector2 globalPos = getGlobalPosition();
        raylib::Rectangle statusbarRect(globalPos.x, globalPos.y, bounds.width, WINDOW_STATUSBAR_HEIGHT);
        if(statusbarRect.CheckCollision(::GetMousePosition()))
        {
            // grab with mouse
            holdingMouse = true;
        }
    }
    if(holdingMouse)
    {
        if(IsMouseButtonReleased(0))
        {
            // release
            holdingMouse = false;
        }
        else
        {
            // drag
            setPosition(getGlobalPosition() + ::GetMouseDelta());
        }
    }

    if(::GuiWindowBox(bounds, title.c_str()))
    {
        destroy();
    }   

    UI_DRAW_OVERRIDE
}


//--- MultiTextBox ---//

nano::MultiTextBox::MultiTextBox(std::string name) : Label(name) {}
nano::MultiTextBox::MultiTextBox(std::string name, std::string text) : Label(name, text) {}
nano::MultiTextBox::MultiTextBox(std::string name, raylib::Rectangle bounds, raylib::Color backgroundColor, std::string text) : 
    Label(name, bounds, backgroundColor, text) {}
nano::MultiTextBox::MultiTextBox(std::string name, std::string text, raylib::Color textColor, uint8_t fontSize):
    Label(name, text, textColor, fontSize) {}

void nano::MultiTextBox::draw()
{
    if(!visible) return;

    raylib::Rectangle dest(getGlobalPosition(), getSize());
    ::GuiLoadStyleDefault();
    ::GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    ::GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, textColor);
    ::GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, textColor);
    ::GuiSetStyle(TEXTBOX, BORDER_WIDTH, borderWidth);
    ::GuiSetStyle(TEXTBOX, BACKGROUND_COLOR, backgroundColor);
    ::GuiSetStyle(TEXTBOX, TEXT_INNER_PADDING, 0);
    ::GuiFade(textColor.a / 255.0f);
    ::GuiTextBoxMulti(dest, const_cast<char*>(text.c_str()), text.length(), false);
    
    UI_DRAW_OVERRIDE
}


//--- ProgressBar ---//

nano::ProgressBar::ProgressBar(std::string name, raylib::Vector2 position) : ProgressBar(name, position, 0.0f, 1.0f, "") {}

nano::ProgressBar::ProgressBar(std::string name, raylib::Vector2 position, float minValue, float maxValue, std::string textTop) : ProgressBar(name, position, minValue, maxValue, textTop,"", "") {}

nano::ProgressBar::ProgressBar(std::string name, raylib::Vector2 position, float minValue, float maxValue, std::string textTop, std::string textLeft, std::string textRight) :
    Control(name), minValue(minValue), maxValue(maxValue), textTop(textTop), textLeft(textLeft), textRight(textRight), value(minValue)
    {
        bounds = raylib::Rectangle(position, { width, height });
        backgroundColor = BLACK;
    }

void nano::ProgressBar::draw()
{
    if(!visible) return;

    raylib::Rectangle dest;
    dest.SetSize(width, height);
    if(centralize)
    {
        raylib::Vector2 parentSize = parent->getSize();
        dest.SetPosition(parent->getGlobalPosition() + (parentSize - getSize()) * 0.5f);
    }
    else
    {
        dest.SetPosition(getGlobalPosition());
    }
    ::DrawRectangleRec(dest,  backgroundColor);
    ::GuiProgressBar(dest, textLeft.c_str(), textRight.c_str(), value, minValue, maxValue);

    ::GuiLoadStyleDefault();
    ::GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, raylib::Color::White());
    ::GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    dest.y -= dest.height;
    ::GuiLabel(dest, textTop.c_str());

    UI_DRAW_OVERRIDE
}


//--- ValueInput ---//

nano::ValueInput::ValueInput(std::string name, uint8_t length, raylib::Vector2 position) : ValueInput(name, length, position, 0.0f, 1.0f) {}

nano::ValueInput::ValueInput(std::string name, uint8_t length, raylib::Vector2 position, float minValue, float maxValue, std::string label) :
    Control(name), length(length), minValue(minValue), maxValue(maxValue), label(label)
    {
        bounds.SetPosition(position);
        backgroundColor = GRAY;
    }

nano::ValueInput::~ValueInput()
{
    delete[] textValue;
}

void nano::ValueInput::draw()
{
    if(!visible) return;

    ::GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);

    raylib::Rectangle dest;
    raylib::Vector2 textSize(::MeasureTextEx(::GetFontDefault(), "8", fontSize, 1));
    dest.SetPosition(getGlobalPosition());
    dest.SetSize((length + 1u) * textSize.x, textSize.y + TEXT_PADDING);   // +1 for dot
    
    if(!editMode)
    {
        if(::IsMouseButtonPressed(0) && dest.CheckCollision(::GetMousePosition()))
        {
            editMode = true;
        }
    }
    else
    {
        if(::IsMouseButtonPressed(0) && !dest.CheckCollision(::GetMousePosition()))
        {
            editMode = false;
        }
    }

    if((textValue[0] >= 48) && (textValue[0] <= 57))
    {
        value = std::stof(textValue);
        value = std::clamp(value, minValue, maxValue);
    }

    ::DrawRectangleRec(dest,  backgroundColor);
    ::GuiTextBox(dest, textValue, length + 1u, editMode);
    dest.x -= dest.width;
    ::GuiLabel(dest, label.c_str());

    UI_DRAW_OVERRIDE
}


//--- ScrollPanel ---//

nano::ScrollPanel::ScrollPanel(std::string name, raylib::Rectangle bounds) :
    ScrollPanel(name, bounds, defaultBackgroundColor, raylib::Vector2(0.0f,0.0f)) {}

nano::ScrollPanel::ScrollPanel(std::string name, raylib::Rectangle bounds, raylib::Color backgroundColor, raylib::Vector2 contentSize) :
    Control(name, bounds, backgroundColor), contentSize(contentSize) {}

void nano::ScrollPanel::draw()
{
    if(!visible) return;

    raylib::Rectangle dest(getGlobalPosition(), bounds.GetSize());
    raylib::Rectangle contentRec;
    contentRec.SetSize(contentSize);
    ::GuiLoadStyleDefault();
    ::GuiSetStyle(DEFAULT, BACKGROUND_COLOR, backgroundColor);
    ::GuiScrollPanel(dest, contentRec, &scroll);

    UI_DRAW_OVERRIDE
}