#include "app.hpp"
#include "debug.hpp"
#include "prim_exception.hpp"
#include "imgui_wrappers.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <rlImGui.h>
#include <fstream>

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
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    defaultFont = io.Fonts->AddFontFromFileTTF("./res/fonts/Kanit-Medium.ttf", defaultFontSize);
    rlImGuiReloadFonts();

    // TEST

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
            if(currImg.IsReady())
            {
                ImGui::Text(currImgPath.c_str());
                ImGui::Separator();
            }
            const float mainPanelHeight = 200.0f;
            ImGui::SetWindowPos({ 0.0f, (float)window.GetHeight() - mainPanelHeight});
            ImGui::SetWindowSize({ (float)window.GetWidth(), mainPanelHeight});
            if(currImg.IsReady() && ImGui::Button("Start extremum analysis")) 
            {
                extremumAnalysisConfigVisible = !extremumAnalysisConfigVisible;
                menuVisible = false;
            }
            ImGui::SameLine();
            if(ImGui::Button("Console")) consoleVisible = !consoleVisible;
            ImGui::SetTooltipD("show/hide console", 1.0f);
            if(maskTexture && ImGui::Button("Mask")) maskVisible = !maskVisible;
            ImGui::End();
        }

        if(!currImg.IsReady())
        {
            static const char* text = "Drop an image to start analysis";
            static const uint8_t fontSize = 28u;
            static const Vector2 fontSizeVec = ::MeasureTextDefaultFont(text, fontSize);
            DrawText(text, (window.GetWidth() - fontSizeVec.x) * 0.5f, (window.GetHeight() - fontSizeVec.y) * 0.5, fontSize, WHITE);
        }

        if(extremumAnalysisConfigVisible)
        {
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            ImGui::Begin("Extremum analysis config", &extremumAnalysisConfigVisible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
            {
                ImGui::PushItemWidth(100.0f);
                if(currImg.IsReady())
                {
                    ImGui::InputFloatClamped("Pixel size (nm)", &analyser.pixelSize_nm, 0.0f);
                    ImGui::InputFloatClamped("Delta step", &analyser.extremumDeltaStep, 0.0f);
                    if(analyser.processFullRange) ImGui::BeginDisabled();
                    ImGui::InputIntClamped("Overflow tolerance", &analyser.extremumOverflowTolerance, 0);
                    if(analyser.processFullRange) ImGui::EndDisabled();
                    ImGui::Checkbox("Process full range", &analyser.processFullRange);
                    ImGui::InputIntClamped("Min pixels in nanotube", &analyser.minPixelsInTube, 0);
                }
                else
                {
                    ImGui::Text("No image to analyse!");
                }
                ImGui::PopItemWidth();
                if(ImGui::Button("Start"))
                {
                    extremumAnalysisConfigVisible = false;
                    startAnalysis();
                }
            }
            ImGui::End();
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
            ImGui::Begin("Console", &consoleVisible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);

            if(ImGui::BeginMenuBar());
            {
                if(ImGui::BeginMenu("File"))
                {
                    ImGui::MenuItem("Clear", NULL, &clearConsole);
                    ImGui::MenuItem("Save to file", NULL, &consoleToFile);
                    ImGui::EndMenu();
                }
                
                ImGui::EndMenuBar();
            }
         

            ImGui::TextUnformatted(consoleBuffer.begin());
            if(consoleScrollToBottom) 
            {
                ImGui::SetScrollHereY(1.0f);
                consoleScrollToBottom = false;
            }
            ImGui::End();
        }

        if(alertWindowVisible)
        {
            ImGui::OpenPopup("Alert!");
            ImGui::SetWindowSize({ 0.0f, 0.0f });
            // Always center this window when appearing
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Alert!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text(alertText.c_str());
                ImGui::Separator();  

                if (ImGui::Button("OK")) 
                {
                    ImGui::CloseCurrentPopup(); 
                    alertWindowVisible = false;
                } 
                ImGui::SetItemDefaultFocus();
                ImGui::EndPopup();
            }

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
    alertWindowVisible = true;
    alertText = std::move(message);
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
            if(maskTexture && maskVisible) maskTexture->Draw(cameraPosition, 0.0f, cameraZoom);

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


    // flags processing
    if(clearConsole) consoleBuffer.clear(); clearConsole = false;
    if(consoleToFile)
    {
        if(consoleBuffer.empty())
        {
            alert("The console is empty.");
        }
        else
        {
            std::ofstream os("./results.txt");
            if(os.good())
            {
                os << consoleBuffer.c_str();
                os.close();
                alert("File results.txt saved!");
                
            }
            else
            {
                throw PRIM_EXCEPTION("Couldn't save file.");
            }
        }
       consoleToFile = false;
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

    printLine("<<<<< Starting analysis >>>>>");
   
    auto workerLambda = [this]() {
            this->analyser.findExtremum();
            this->workerIsDone = true;
        };

    worker = std::thread(workerLambda);
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
    consoleBuffer.append(line.c_str());
    consoleScrollToBottom = true;
}

void nano::App::printLine(std::string line)
{
    consoleBuffer.append(("\n" + line).c_str());
    consoleScrollToBottom = true;
}

std::string nano::App::floatToString(float f, uint8_t precision)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << f;
    return std::move(ss.str());
}
