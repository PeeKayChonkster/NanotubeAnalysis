#ifndef APP_HPP
#define APP_HPP

#include <raylib-cpp.hpp>
#include "nanotube.hpp"
#include "analyser.hpp"
#include "ui.hpp"
#include <memory>
#include <algorithm>
#include <thread>

namespace nano
{

class App
{
private:
    raylib::Window window;
    raylib::Image currImg;
    std::string currImgPath;
    raylib::Texture* currTexture = nullptr;
    raylib::Texture* maskTexture = nullptr;
    Analyser analyser;
    Control* uiRoot;
    raylib::Vector2 cameraPosition {};
    float cameraZoom = 1.0f;
    std::thread worker;

    // flags
    bool workerIsDone = false;

    char inputThreshold[4] {0};
    bool calculateButtonPressed = false;
    const uint8_t uiFontSize = 20;
    raylib::Font uiFont;

    int init();
    void drawUI();
    void setDroppedImg();
    void setWindowSize(raylib::Vector2 size);
    void alert(std::string message);
    void setMaskTexture();


    template<class T, class... Args>
    T* createUIElement(Args&&... args)
    {
        T* uiElement = new T(std::forward<Args>(args)...);
        uiRoot->addChild(uiElement);
        return uiElement;
    }

    void processControls();
    void startAnalysis();

public:
    App(int windowWidth, int widnowHeight, const char* windowName);
    App(const App& other) = delete;

    int run();
    void free();
};

} // namespace nano
#endif // APP_HPP