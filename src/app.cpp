#include "app.hpp"
#include "debug.hpp"
#include <iostream>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

nano::App::App(int windowWidth, int windowHeight, const char* windowName): 
    window(windowWidth, windowHeight, windowName), 
    uiRoot("root", { 0, 0, windowWidth, windowHeight }) 
    {}

int nano::App::init()
{
    SetTargetFPS(60);

    // GUI initialization
    // ::GuiLoadStyle("./res/ui_styles/nano.rgs");
    // uiFont = ::LoadFontEx("./res/fonts/Lato-Regular.ttf", uiFontSize, NULL, 0);
    // ::GuiSetFont(uiFont);
    Control* mainPanel = createUIElement<Control>("mainPanel", raylib::Rectangle(), WHITE);
    mainPanel->visible = false;
    Label* dropInfo = createUIElement<Label>("dropInfo", raylib::Rectangle(), BLANK, "Drop image file for analysis");
    dropInfo->fontSize = 22u;
    dropInfo->sizePolicy = SizePolicy::SHRINK;
    uiRoot.addChild(mainPanel);
    uiRoot.addChild(dropInfo);


    return 0;
}
    
void nano::App::drawUI()
{
    auto mainPanel = uiRoot.getChild<Control>("mainPanel");
    if(::IsKeyPressed(KEY_TAB)) mainPanel->visible = !mainPanel->visible;
    if(mainPanel->visible)
    {
        mainPanel->setSize({ window.GetWidth(), window.GetHeight() / 4.0f });
        mainPanel->setPosition({ 0, window.GetHeight() - mainPanel->getHeight() });
    }

    auto dropInfo = uiRoot.getChild<Label>("dropInfo");
    if(!currImg.IsReady())
    {
        dropInfo->centralize(&window);
    }
    else
    {
        dropInfo->visible = false;
    }

    uiRoot.draw();
    // if(!analyser.getTubes()->empty())
    // {
    //     std::string str("Nanotubes: " + std::to_string(analyser.getTubes()->size()));
    //     raylib::Rectangle numberOfTubesRec(5, 5, MeasureTextEx(uiFont, str.c_str(), uiFontSize, 1.0f).x, 20);
    //     ::GuiDrawRectangle(numberOfTubesRec, 1, BLACK, WHITE);
    //     ::GuiLabel(numberOfTubesRec, str.c_str());
    // }

    // raylib::Rectangle calculateBtnRec { 5, 50, 80, 30 };
    // if(currImg.IsReady())
    // {
    //     if(::GuiButton(calculateBtnRec, "Calculate"))
    //     {
    //         analyser.findExtremum();
    //         if(maskTexture) delete maskTexture;
    //         maskTexture = new raylib::Texture(*analyser.getMask());
    //     }
    // }
    // else
    // {
    //     DrawText("Drop an image to analyse it", (window.GetSize().x - MeasureText("Drop an image to analyse it", 26)) * 0.5, window.GetSize().y / 2.0f - 14, 26, WHITE);
    // }
}

void nano::App::setDroppedImg()
{
    int numberOfFiles;
    char** droppedFiles = GetDroppedFiles(&numberOfFiles);
    if(numberOfFiles > 0)
    {
        currImg = raylib::Image(droppedFiles[0]);
        currImg.Format(PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
        if(currTexture) delete currTexture;
        currTexture = new raylib::Texture(currImg);
        window.SetSize(currImg.GetSize());
        analyser.setTargetImg(&currImg);
        std::cout << "Dropped file path: " << droppedFiles[0] << std::endl;
        ClearDroppedFiles();
    }

}

void nano::App::setWindowSize(raylib::Vector2 size)
{
    window.SetSize(size);
    uiRoot.setSize(size);
}

void nano::App::alert(std::string message)
{
    ////////////////////////
    // UNDER CONSTRUCTION //
    ////////////////////////
}

int nano::App::run()
{
    int initCode = init();
    if(initCode != 0) return initCode;

    while(!window.ShouldClose())
    {
        //--- UPDATE ---//
        if(IsFileDropped()) setDroppedImg();
        //--------------//


        //--- DRAW ---//
        window.BeginDrawing();
        window.ClearBackground();


        if(currTexture) currTexture->Draw();
        if(maskTexture) maskTexture->Draw();

        drawUI();

        prim::Debug::draw(RED);
        
        window.EndDrawing();
        //--------------//
    }

    App::free();
    return 0;
}

void nano::App::free()
{
    delete currTexture;
    delete maskTexture;
}