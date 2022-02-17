#ifndef APP_HPP
#define APP_HPP

#include <raylib-cpp.hpp>
#include "nanotube.hpp"

namespace nano
{

class App
{
private:
    raylib::Window window;

    int init();
    void drawUI();
    char inputThreshold[4] = {0};
    bool calculateButtonPressed = false;
public:
    App(int windowWidth, int widnowHeight, const char* windowName);
    App(const App& other) = delete;

    int run();
};

} // namespace nano
#endif // APP_HPP