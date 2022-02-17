#include "app.hpp"
#include <iostream>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

nano::App::App(int windowWidth, int windowHeight, const char* windowName): window(windowWidth, windowHeight, windowName) {}

int nano::App::init()
{
    SetTargetFPS(30);
    return 0;
}
    

int nano::App::run()
{
    int initCode = init();
    if(initCode != 0) return initCode;

    float threshold = 0.6f;
    raylib::Image tempImg("./res/img/test.jpg");
    tempImg.Format(PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
    raylib::Texture tempTexture(tempImg);
    raylib::Image mask(createValueMask(tempImg, 0.5));
    raylib::Texture maskTexture(mask);
    window.SetSize(tempTexture.GetSize());

    while(!window.ShouldClose())
    {
        //--- UPDATE ---//
        mask = createValueMask(tempImg, threshold);
        //--------------//


        //--- DRAW ---//
        window.BeginDrawing();
        window.ClearBackground();

        

        tempTexture.Draw();
        maskTexture.Update(mask.data);
        maskTexture.Draw();

        threshold = ::GuiSliderBar({ 50, 50, 300, 30 }, "0", "1", threshold, 0.0f, 1.0f);

        window.EndDrawing();
        //--------------//
    }

    return 0;
}