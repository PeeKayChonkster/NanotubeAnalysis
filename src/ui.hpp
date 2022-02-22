#ifndef UI_HPP
#define UI_HPP

#include <raylib-cpp.hpp>
#include <raygui.h>
#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include "prim_exception.hpp"

namespace nano
{

#define UI_DRAW_OVERRIDE drawChildren();

#define WINDOW_STATUSBAR_HEIGHT 22


class Control
{
protected:
    std::string name;
    void drawChildren();

    static const uint8_t defaultFontSize = 20u;

public:
    Control(std::string name);
    Control(std::string name, raylib::Rectangle bounds, raylib::Color backgroundColor = BLANK);
    virtual ~Control() = default;

    raylib::Rectangle bounds;
    raylib::Color backgroundColor;
    std::vector<Control*> children;
    Control* parent = nullptr;
    bool visible = true;

    void addChild(Control* child);
    void removeChild(Control* child);
    void destroy();
    Control* getChild(std::string name);
    std::string getName() const;
    raylib::Vector2 getPosition() const;
    raylib::Vector2 getGlobalPosition() const;
    void setPosition(raylib::Vector2 position);
    raylib::Vector2 getSize() const;
    void setSize(raylib::Vector2 size);
    int getWidth() const;
    int getHeight() const;
    void setWidth(uint16_t width);
    void setHeight(uint16_t width);
    raylib::Color getBackgroundColor() const;
    void setBackgroundColor(raylib::Color color);
    void centralize();

    virtual void draw();

    template<class T>
    inline T* getChild(std::string name) const
    {
        T* child = nullptr;
        for(int i = 0; i < children.size(); ++i)
        {
            if(children[i]->name == name)
            {
                child = dynamic_cast<T*>(children[i]);
                if(child) return child;
            }
        }
        for(const auto& c : children)
        {enum class SizePolicy { FIXED, EXPAND, SHRINK };
            child = c->getChild<T>(name);
            if(child) return child;
        }
        throw PRIM_EXCEPTION("Trying to get non-existent child.");
        return nullptr;
    }


};


class Label : public Control
{
protected:
    std::string text;
public:
    Label(std::string name);
    Label(std::string name, std::string text);
    Label(std::string name, raylib::Rectangle bounds, raylib::Color backgroundColor, std::string text);
    Label(std::string name, std::string text, raylib::Color textColor, uint8_t fontSize);
    virtual ~Label() = default;

    uint8_t fontSize = defaultFontSize;
    raylib::Color textColor = WHITE;


    std::string_view getText() const;
    void setText(std::string text);


    virtual void draw() override;
};


class Button : public Label
{
protected:
    bool pressed = false;
    std::function<void()> callback;
public:
    Button(std::string name);
    Button(std::string name, std::string text);
    Button(std::string name, raylib::Rectangle bounds, raylib::Color backgroundColor, std::string text);
    virtual ~Button() = default;

    bool shrink = false;

    inline bool isPressed() const { return pressed; }
    inline void setCallback(std::function<void()> callback) { this->callback = callback; }
    
    virtual void draw() override;
};


class WindowBox: public Control
{
private:
    std::string title;
    bool holdingMouse = false;
public:
    WindowBox(std::string name);
    WindowBox(std::string name, std::string title);
    WindowBox(std::string name, raylib::Rectangle bounds, raylib::Color backgroundColor, std::string title);
    virtual ~WindowBox() = default;

    virtual void draw() override;
};


class MultiTextBox : public Label
{
public:
    MultiTextBox(std::string name);
    MultiTextBox(std::string name, std::string text);
    MultiTextBox(std::string name, raylib::Rectangle bounds, raylib::Color backgroundColor, std::string text);
    MultiTextBox(std::string name, std::string text, raylib::Color textColor, uint8_t fontSize);
    virtual ~MultiTextBox() = default;


    uint8_t innerPadding;
    uint8_t borderWidth = 3u;
    //bool editMode = false;    always false for now

    virtual void draw() override;
};


class ProgressBar : public Control
{
private:
    float minValue;
    float maxValue;
    std::string textLeft;
    std::string textRight;

public:
    ProgressBar(std::string name, raylib::Vector2 position);
    ProgressBar(std::string name, raylib::Vector2 position, float minValue, float maxValue);
    ProgressBar(std::string name, raylib::Vector2 position, float minValue, float maxValue, std::string textLeft, std::string textRight);
    virtual ~ProgressBar() = default;

    virtual void draw() override;

    float value;
    bool centralize = true;
    const float width = 250.0f;
    const float height = 50.0f;
};


} // namespace nano

#endif // UI_HPP