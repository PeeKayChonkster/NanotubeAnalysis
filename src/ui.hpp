#ifndef UI_HPP
#define UI_HPP

#include <raylib-cpp.hpp>
#include <raygui.h>
#include <string>
#include <string_view>
#include <vector>
#include "prim_exception.hpp"

namespace nano
{

#define UI_DRAW_OVERRIDE if(!visible) return; drawChildren();

enum class SizePolicy { FIXED, EXPAND, SHRINK };


class Control
{
protected:
    std::string name;
    std::vector<Control*> children;
    Control* parent = nullptr;
    void drawChildren();

public:
    Control(std::string name);
    Control(std::string name, raylib::Rectangle boundingRect, raylib::Color backgroundColor = BLANK);

    raylib::Rectangle boundingRect;
    raylib::Color backgroundColor;
    bool visible = true;
    SizePolicy sizePolicy = SizePolicy::FIXED;

    void addChild(Control* child);
    void removeChild(Control* chlild);
    Control* getChild(std::string name);
    std::string getName() const;
    raylib::Vector2 getPosition() const;
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
    void centralize(const Control* box);
    void centralize(const raylib::Window* window);

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
        {
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
    Label(std::string name, raylib::Rectangle boundingRect, raylib::Color backgroundColor, std::string text);

    uint8_t fontSize = 16u;
    raylib::Color textColor = WHITE;


    std::string_view getText() const;
    void setText(std::string text);


    virtual void draw() override;
};

} // namespace nano

#endif // UI_HPP