#include <filesystem>
#include <iostream>
#include <algorithm>
#include <imgui.h>
#include <raygui.h>
#define RAYGUI_IMPLEMENTATION

#include "app.hpp"
#include "debug.hpp"
#include "ui.hpp"
#include "prim_exception.hpp"


int nano::App::init(int windowWidth, int widnowHeight, const char* windowName)
{
    window.SetSize(windowWidth, widnowHeight);
    window.SetTitle(windowName);
    SetTargetFPS(60);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    return 0;
}

void nano::App::setDroppedImg()
{
    int numberOfFiles;
    char** droppedFiles = GetDroppedFiles(&numberOfFiles);
    if(numberOfFiles > 0)
    {
        std::string path = std::filesystem::u8path(droppedFiles[0]);
        currImg.Load(path);
        currImgPath = path;
        currImg.Format(PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
        if(currTexture) delete currTexture;
        currTexture = new raylib::Texture(currImg);
        setWindowSize(currImg.GetSize());
        analyser.resetAll();
        analyser.setTargetImg(&currImg);
        if(maskTexture)
        {
            delete maskTexture;
            maskTexture = nullptr;
        }
        std::cout << "Dropped file path: " << path << std::endl;
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



int nano::App::run(int windowWidth, int windowHeight, const char* windowName)
{
    int initCode = init(windowWidth, windowHeight, windowName);
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
            if(maskTexture && maskVisible) maskTexture->Draw(cameraPosition, 0.0f, cameraZoom);

            prim::Debug::draw(RED);
            
            UI::inst().draw();
        }
        EndDrawing();

        //--------------//
    }

    return 0;
}

void nano::App::processControls()
{
    raylib::Vector2 mousePos = ::GetMousePosition();
    //prim::Debug::printLine(std::to_string(mousePos.x) + "|" + std::to_string(mousePos.y));

    // if mouse NOT hovering above any gui elements
    if(!UI::inst().wantCaptureMouse())
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
   
}

void nano::App::startAnalysis()
{
    if(!currImg.IsReady())
    {
        UI::inst().alert("There is no image to analyse!");
        return;
    }
    
    maskVisible = false;

    calculating = true;
    UI::inst().consoleVisible = true;

    UI::inst().printLine("<<<<< Starting analysis >>>>>");
    UI::inst().printLine("Image: " + currImgPath);

    auto workerLambda = []() {
            analyser.findExtremum();
            workerIsDone = true;
        };
    worker = std::thread(workerLambda);
}

void nano::App::cancelAnalysis()
{
    if(calculating)
    {
        analyser.cancelAnalysis();
        if(maskTexture)
        {
            delete maskTexture;
            maskTexture = nullptr;
        }
    }
}

void nano::App::setMaskTexture()
{
    if(analyser.getMask()->IsReady())
    {
        if(maskTexture)
        {
            maskTexture->Update(analyser.getMask()->GetData());
        }
        else
        {
            maskTexture = new raylib::Texture(*analyser.getMask());
        }

        maskVisible = true;
    }
}

void nano::App::free()
{
    if(currTexture) delete currTexture;
    if(maskTexture) delete maskTexture;
    if(worker.joinable()) worker.join();
}