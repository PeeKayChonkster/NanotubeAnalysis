#include "app.hpp"
#include "debug.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <rlImGui.h>

#include <raygui.h>
#define RAYGUI_IMPLEMENTATION

nano::App::App(int windowWidth, int windowHeight, const char* windowName): 
    window(windowWidth, windowHeight, windowName),
    analyser(this)
    {}

int nano::App::init()
{
    SetTargetFPS(60);
    window.SetState(FLAG_WINDOW_RESIZABLE);
    

    // GUI SETUP //
    rlImGuiSetup(true);
    ImGuiIO& io = ImGui::GetIO();
    defaultFont = io.Fonts->AddFontFromFileTTF("./res/fonts/Kanit-Medium.ttf", defaultFontSize);
    rlImGuiReloadFonts();


    return 0;
}

void nano::App::drawUI()
{
    rlImGuiBegin();
    {
        ImGui::PushFont(defaultFont);

        //ImGui::ShowDemoWindow();
        //ImGui::ShowStyleEditor();

        if(menuVisible)
        {
            ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
            const float mainPanelHeight = 200.0f;
            ImGui::SetWindowPos({ 0.0f, (float)window.GetHeight() - mainPanelHeight});
            ImGui::SetWindowSize({ (float)window.GetWidth(), mainPanelHeight});
            if(ImGui::Button("Calculate")) startAnalysis();
            ImGui::SameLine();
            if(ImGui::Button("Console")) consoleVisible = !consoleVisible;
            ImGui::PushItemWidth(100.0f);
            ImGui::InputFloat("Pixel size (nm)", &analyser.getPixelSize());
            ImGui::PopItemWidth();
            ImGui::End();
        }

        if(!currImg.IsReady())
        {
            static const char* text = "Drop an image to start analysis";
            static const uint8_t fontSize = 28u;
            static const Vector2 fontSizeVec = ::MeasureTextDefaultFont(text, fontSize);
            DrawText(text, (window.GetWidth() - fontSizeVec.x) * 0.5f, (window.GetHeight() - fontSizeVec.y) * 0.5, fontSize, WHITE);
        }

        if(calculating)
        {
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            //ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImGui::GetStyleColorVec4(ImGuiCol_TitleBgActive));
            //ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetStyleColorVec4(ImGuiCol_TitleBg));
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(center.x * 0.5f, 0.0f), ImGuiCond_Appearing);
            ImGui::Begin("Calculating...", NULL,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
            ImGui::ProgressBar(analyser.getProgress());
            //ImGui::PopStyleColor(2);
            ImGui::End();
        }

        if(consoleVisible)
        {
            ImGui::Begin("Console", &consoleVisible, ImGuiWindowFlags_NoCollapse);
            ImGui::TextWrapped(consoleBuffer.c_str());
            if(consoleScrollToBottom) 
            {
                ImGui::SetScrollHereY(1.0f);
                consoleScrollToBottom = false;
            }
            ImGui::End();
        }

        if(alertWindowVisible)
        {
            // TBI
        }

        ImGui::PopFont();
    }
    rlImGuiEnd();
}

void nano::App::setDroppedImg()
{
    int numberOfFiles;
    char** droppedFiles = GetDroppedFiles(&numberOfFiles);
    if(numberOfFiles > 0)
    {
        currImg.Load(droppedFiles[0]);
        currImgPath = std::move(droppedFiles[0]);
        currImg.Format(PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
        if(currTexture) delete currTexture;
        currTexture = new raylib::Texture(currImg);
        setWindowSize(currImg.GetSize());
        analyser.resetAll();
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
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = (float)window.GetWidth();
    io.DisplaySize.y = (float)window.GetHeight();
}

void nano::App::alert(std::string message)
{
    // TO BE IMPLEMENTED
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
        if(IsFileDropped()) setDroppedImg();    // process dropped files
        processControls();                      // process controls
        if(workerIsDone)                        // free worker thread
        {
            worker.join();
            workerIsDone = calculating = false;
            setMaskTexture();
        }
        //--------------//


        //--- DRAW ---//
        BeginDrawing();
        {
            window.ClearBackground();

            if(currTexture) currTexture->Draw(cameraPosition, 0.0f, cameraZoom);
            if(maskTexture) maskTexture->Draw(cameraPosition, 0.0f, cameraZoom);

            prim::Debug::draw(RED);
            
            drawUI();
        }
        EndDrawing();

        //--------------//
    }

    App::free();
    return 0;
}

void nano::App::free()
{
    delete currTexture;
    delete maskTexture;

    rlImGuiShutdown();
}

void nano::App::processControls()
{
    raylib::Vector2 mousePos = ::GetMousePosition();
    ImGuiIO& io = ImGui::GetIO();
    //prim::Debug::printLine(std::to_string(mousePos.x) + "|" + std::to_string(mousePos.y));

    // if mouse NOT hovering above any gui elements
    if(!io.WantCaptureMouse)
    {
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
   

    if(::IsKeyPressed(KEY_TAB))
    {
        menuVisible = !menuVisible;
    }

}

void nano::App::startAnalysis()
{
    if(!currImg.IsReady())
    {
        alert("There is no image to analyse!");
        return;
    }
    
    calculating = true;
    consoleVisible = true;

    printLine("Image area = " + floatToString(analyser.getImageArea() * 0.000001, 3u) + " mm2");

    worker = std::thread([this]() {
        this->analyser.findExtremum();
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

void nano::App::print(std::string line)
{
    consoleBuffer += line;
    consoleScrollToBottom = true;
}

void nano::App::printLine(std::string line)
{
    consoleBuffer += "\n" + line;
    consoleScrollToBottom = true;
}

std::string nano::App::floatToString(float f, uint8_t precision)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << f;
    return std::move(ss.str());
}
