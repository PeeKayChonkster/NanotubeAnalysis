#ifndef APP_HPP
#define APP_HPP

#include <raylib-cpp.hpp>
#include "nanotube.hpp"
#include "analyser.hpp"
#include "ui.hpp"
#include <memory>

namespace nano
{

class App
{
private:
    raylib::Window window;
    raylib::Image currImg;
    raylib::Texture* currTexture = nullptr;
    raylib::Texture* maskTexture = nullptr;
    Analyser analyser;
    Control uiRoot;
    std::vector<std::unique_ptr<Control>> uiElements;

    // flags
    bool calculating = false;

    char inputThreshold[4] {0};
    bool calculateButtonPressed = false;
    const uint8_t uiFontSize = 20;
    raylib::Font uiFont;

    int init();
    void drawUI();
    void setDroppedImg();
    void setWindowSize(raylib::Vector2 size);
    void alert(std::string message);
    void free();

    template<class T, class... Args>
    T* createUIElement(Args&&... args)
    {
        T* uiElelement = new T(std::forward<Args>(args)...);
        uiElements.emplace_back(uiElelement);
        return uiElelement;
    }

public:
    App(int windowWidth, int widnowHeight, const char* windowName);
    App(const App& other) = delete;

    int run();
};

} // namespace nano
#endif // APP_HPP