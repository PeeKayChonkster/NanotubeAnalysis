#ifndef APP_HPP
#define APP_HPP

#include <raylib-cpp.hpp>
#include "nanotube.hpp"
#include "analyser.hpp"
#include <memory>
#include <algorithm>
#include <thread>
#include <imgui.h>
#include <cassert>

// #define WHITE { 1.0f, 1.0f, 1.0f, 1.0f }
// #define BLACK { 0.0f, 0.0f, 0.0f, 1.0f }
// #define RED { 1.0f, 0.0f, 0.0f, 1.0f }
// #define GREEN { 0.0f, 1.0f, 0.0f, 1.0f }
// #define BLUE { 0.0f, 0.0f, 1.0f, 1.0f }
// #define YELLOW { 0.0f, 1.0f, 1.0f, 1.0f }
// #define MAGENTA { 1.0f, 0.0f, 1.0f, 1.0f }

namespace nano
{

enum ConfigValue { PixelSize, DeltaStep, OverflowTolerance };

class App
{
private:
    raylib::Window window;
    raylib::Image currImg;
    std::string currImgPath;
    raylib::Texture* currTexture = nullptr;
    raylib::Texture* maskTexture = nullptr;
    Analyser analyser;
    raylib::Vector2 cameraPosition {};
    float cameraZoom = 1.0f;
    const float defaultFontSize = 20.0f;
    ImFont* defaultFont = nullptr;
    std::thread worker;
    ImGuiTextBuffer consoleBuffer;
    std::string alertText;
    const float tooltipDelay = 1.0f;

    // flags
    bool workerIsDone = false;
    bool calculating = false;
    bool menuVisible = false;
    bool alertWindowVisible = false;
    bool consoleVisible = false;
    bool consoleScrollToBottom = false;
    bool clearConsole = false;
    bool consoleToFile = false;
    bool maskVisible = true;
    bool extremumAnalysisConfigVisible = false;
    //

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

    void processControls();
    void startAnalysis();

public:
    App(int windowWidth, int widnowHeight, const char* windowName);
    App(const App& other) = delete;

    int run();
    void free();
    void print(std::string line);
    void printLine(std::string line);
    static std::string floatToString(float f, uint8_t precision);
};

} // namespace nano
#endif // APP_HPP