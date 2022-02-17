#include "app.hpp"
#include "analyser.hpp"
#include "debug.hpp"
#include <iostream>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

nano::App::App(int windowWidth, int windowHeight, const char* windowName): window(windowWidth, windowHeight, windowName) {}

int nano::App::init()
{
    SetTargetFPS(30);
    return 0;
}
    
void nano::App::drawUI()
{
    ::GuiLabel({ 5, 100, 70, 30}, "Alpha Threshold");
    ::GuiTextBox({ 100, 100, 50, 20 }, inputThreshold, 6, true); 
    calculateButtonPressed = ::GuiButton({ 5, 150, 100, 30 }, "Calculate");
}

int nano::App::run()
{
    int initCode = init();

    if(initCode != 0) return initCode;
    float threshold = 0.9f;
    raylib::Image mainImg("./res/img/test.jpg");
    mainImg.Format(PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
    Analyser analyser(&mainImg);
    analyser.calculateMask(threshold);
    analyser.findNanotubes();
    raylib::Texture mainTexture(mainImg);
    raylib::Texture maskTexture(*analyser.getMask());
    window.SetSize(mainTexture.GetSize());

    while(!window.ShouldClose())
    {
        //--- UPDATE ---//

        //--------------//


        //--- DRAW ---//
        window.BeginDrawing();
        window.ClearBackground();


        mainTexture.Draw();
        maskTexture.Draw();

        drawUI();
       
        if(calculateButtonPressed)
        {
            DrawText("Calculating...", window.GetWidth() / 2.0f - GetTextWidth("Calculating..."), window.GetHeight() / 2.0f, 30, BLUE);
            window.EndDrawing();
            float value = std::stof(inputThreshold);
            if (value >= 0.0 && value <= 1.0)
            {
                threshold = value;
                analyser.calculateMask(threshold);
                analyser.findNanotubes();
                maskTexture.Update(analyser.getMask()->GetData());
            }
        }
        else
        {
            DrawText(("Nanotubes: " + std::to_string(analyser.getTubes()->size())).c_str(), 5, 50, 26, BLUE);
            prim::Debug::draw(RED);
            
            window.EndDrawing();
        }
        //--------------//
    }

    return 0;
}