#include "app.hpp"

nano::App::App(int windowWidth, int windowHeight, char* windowName): window(windowWidth, windowHeight, windowName) {}

int nano::App::init()
{
    return 0;
}

int nano::App::run()
{
    int initCode = init();
    if(initCode != 0) return initCode;

    while(!window.ShouldClose())
    {
        window.BeginDrawing();
        window.ClearBackground();


        window.EndDrawing();
    }

    return 0;
}