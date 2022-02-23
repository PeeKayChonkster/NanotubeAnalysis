#include "app.hpp"
#include "debug.hpp"
#include <iostream>
#include <algorithm>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

nano::App::App(int windowWidth, int windowHeight, const char* windowName): 
    window(windowWidth, windowHeight, windowName), 
    uiRoot(new Control("root", raylib::Rectangle { 0, 0, windowWidth, windowHeight }))
    {}

int nano::App::init()
{
    SetTargetFPS(60);

    // GUI SETUP //
    const raylib::Color panelColor(255,255,255, 200);
    Control* mainPanel = createUIElement<Control>("mainPanel", raylib::Rectangle(), panelColor);
    mainPanel->visible = false;
    Label* dropInfo = createUIElement<Label>("dropInfo", "Drop image file for analysis", WHITE, 22u);
    Label* imgPathInfo = createUIElement<Label>("imgPathInfo", "", BLACK, 16u);
    imgPathInfo->backgroundColor = panelColor;
    imgPathInfo->visible = false;
    Button* calcButton = createUIElement<Button>("calcButton", "Calculate");
    calcButton->setCallback([this]()->void{ this->startAnalysis(); });
    mainPanel->addChild(calcButton);

    ValueInput* input = createUIElement<ValueInput>("valueInput", 5u, raylib::Vector2(70.0f, 50.0f), 0.0f, 1.0f, "Test");
    mainPanel->addChild(input);

    ScrollPanel* testScrollPanel = createUIElement<ScrollPanel>("testScrollPanel", raylib::Rectangle(100.0f, 100.0f, 250.0f, 250.0f), GREEN, raylib::Vector2(500.0f, 500.0f));
    

    ////////////////

    return 0;
}
    
void nano::App::drawUI()
{
    auto mainPanel = uiRoot->getChild<Control>("mainPanel");
    if(::IsKeyPressed(KEY_TAB)) mainPanel->visible = !mainPanel->visible;
    if(mainPanel->visible)
    {
        mainPanel->setSize({ window.GetWidth(), window.GetHeight() / 4.0f });
        mainPanel->setPosition({ 0, window.GetHeight() - mainPanel->getHeight() });
    }

    auto dropInfo = uiRoot->getChild<Label>("dropInfo");
    if(!currImg.IsReady())
    {
        dropInfo->centralize();
    }

    uiRoot->draw();
}

void nano::App::setDroppedImg()
{
    int numberOfFiles;
    char** droppedFiles = GetDroppedFiles(&numberOfFiles);
    if(numberOfFiles > 0)
    {
        currImg.Load(droppedFiles[0]);
        currImgPath = std::move(droppedFiles[0]);
        Label* imgPathInfo = uiRoot->getChild<Label>("imgPathInfo");
        imgPathInfo->setText(currImgPath);
        imgPathInfo->visible = true;
        uiRoot->getChild<Label>("dropInfo")->visible = false;
        currImg.Format(PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
        if(currTexture) delete currTexture;
        currTexture = new raylib::Texture(currImg);
        setWindowSize(currImg.GetSize());
        analyser.setTargetImg(&currImg);
        if(maskTexture)
        {
            maskTexture->Unload();
            delete maskTexture;
            maskTexture = nullptr;
        }
        std::cout << "Dropped file path: " << droppedFiles[0] << std::endl;
        cameraPosition = raylib::Vector2::Zero();
        cameraZoom = 1.0f;
        ClearDroppedFiles();
    }

}

void nano::App::setWindowSize(raylib::Vector2 size)
{
    int currentMonitor(::GetCurrentMonitor());
    size.x = std::clamp(size.x, 0.0f, static_cast<float>(::GetMonitorWidth(currentMonitor)) * 0.7f);
    size.y = std::clamp(size.y, 0.0f, static_cast<float>(::GetMonitorHeight(currentMonitor)) * 0.7f);
    window.SetSize(size);
    uiRoot->setSize(size);
}

void nano::App::alert(std::string message)
{
    const uint8_t fontSize = 14u;
    const uint8_t maxWidth = 250u;
    raylib::Vector2 textSize = ::MeasureTextEx(::GetFontDefault(), message.c_str(), fontSize, 1);
    const uint8_t rows = textSize.x / maxWidth + 1u;
    float boxWidth = maxWidth;
    float boxHeight = rows * textSize.y * 2.0f; // rows + distance between rows
    raylib::Rectangle alertRect((window.GetWidth() - boxWidth) * 0.5, (window.GetHeight() - boxHeight) * 0.5f, boxWidth, boxHeight);
    WindowBox* box = createUIElement<WindowBox>("alertBox", alertRect, RED, "Alert!");
    alertRect.y = WINDOW_STATUSBAR_HEIGHT;
    alertRect.x = 0.0f;
    alertRect.height -= WINDOW_STATUSBAR_HEIGHT;
    MultiTextBox* textBox = createUIElement<MultiTextBox>("alertTextBox", alertRect, BLANK, message);
    textBox->textColor = BLACK;
    textBox->fontSize = fontSize;
    box->addChild(textBox);
}

int nano::App::run()
{
    int initCode = init();
    if(initCode != 0) return initCode;

    raylib::Rectangle content(0, 0, 90, 200);
    raylib::Vector2 scroll;


    while(!window.ShouldClose())
    {
        //--- UPDATE ---//
        if(IsFileDropped()) setDroppedImg();
        processControls();
        if(workerIsDone)
        {
            worker.join();
            workerIsDone = false;
            uiRoot->getChild<ProgressBar>("mainProgressBar")->destroy();
            setMaskTexture();
        }
        //--------------//


        //--- DRAW ---//
        window.BeginDrawing();
        window.ClearBackground();


        if(currTexture) currTexture->Draw(cameraPosition, 0.0f, cameraZoom);
        if(maskTexture) maskTexture->Draw(cameraPosition, 0.0f, cameraZoom);

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
    uiRoot->destroy();
}

void nano::App::processControls()
{
    raylib::Vector2 mousePos = ::GetMousePosition();
    //prim::Debug::printLine(std::to_string(mousePos.x) + "|" + std::to_string(mousePos.y));

    raylib::Vector2 mouseDelta = ::GetMouseDelta();
    if(::IsMouseButtonDown(1))
    {  
        cameraPosition += mouseDelta;
    }
    
    float wheelDelta = ::GetMouseWheelMove();
    if(wheelDelta)
    {
        float zoomMultiplier = ::IsKeyDown(KEY_LEFT_SHIFT)? 0.4f : 0.02f;
        cameraZoom += wheelDelta * zoomMultiplier;
        cameraZoom = std::max(0.01f, cameraZoom);
    }

}

void nano::App::startAnalysis()
{
    if(!currImg.IsReady())
    {
        alert("There is no image to analyse!");
        return;
    }

    ProgressBar* mainProgressBar = createUIElement<ProgressBar>("mainProgressBar", raylib::Vector2{0.0f,0.0f}, 0.0f, 1.0f, "Analysing image...");
    
    worker = std::thread([this, mainProgressBar]() {
        this->analyser.findExtremum(&mainProgressBar->value);
        this->workerIsDone = true;
    });
}

void nano::App::setMaskTexture()
{
    if(maskTexture)
    {
        maskTexture->Update(analyser.getMask()->GetData());
    }
    else
    {
        maskTexture = new raylib::Texture(*analyser.getMask());
    }
}