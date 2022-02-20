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

    ::GuiLoadStyle("./res/ui_styles/nano.rgs");
    Font font = ::LoadFontEx("./res/fonts/Lato-Regular.ttf", 14, NULL, 0);
    ::GuiSetFont(font);

    return 0;
}
    
void nano::App::drawUI()
{
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
    analyser.findExtremum();
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
       
        DrawText(("Nanotubes: " + std::to_string(analyser.getTubes()->size())).c_str(), 5, 50, 26, BLUE);
        prim::Debug::draw(RED);
        
        window.EndDrawing();
        //--------------//
    }

    return 0;
}