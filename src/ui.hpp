#ifndef UI_HPP
#define UI_HPP

#include <imgui.h>
#include <string>

namespace nano
{

class UI
{
private:
    class ImFont* defaultFont = nullptr;
    const float defaultFontSize = 18.0f;
    ImGuiTextBuffer consoleBuffer;
    std::string alertText;
    const float tooltipDelay = 1.0f;
    ImGuiIO* io;

    void processControls();

    // helper draw functions
    void drawMainMenu();
    void drawDropInstructions();
    void drawExtremumAnalysisConfig();
    void drawCalculateProgressbar();
    void drawConsole();
    void drawAlertWindow();
public:
    UI();
    ~UI();
    static UI& inst();
    void draw();
    std::string floatToString(float f, uint8_t precision);
    void print(std::string line);
    void printLine(std::string line);
    void alert(std::string message);
    bool wantCaptureMouse() const;

    // flags
    bool menuVisible = false;
    bool alertWindowVisible = false;
    bool consoleVisible = false;
    bool consoleScrollToBottom = false;
    bool clearConsole = false;
    bool consoleToFile = false;
    bool extremumAnalysisConfigVisible = false;
    //
};

}  // namespace nano


#endif // UI_HPP