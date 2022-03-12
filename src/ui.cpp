#include "ui.hpp"
#include "rlImGui.h"
#include <iomanip>
#include <fstream>
#include "prim_exception.hpp"
#include "app.hpp"
#include "imgui_wrappers.hpp"

nano::UI::UI()
{
    rlImGuiSetup(true);
    ImGuiIO& io = ImGui::GetIO();
    this->io = &io;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    defaultFont = io.Fonts->AddFontFromFileTTF("./res/fonts/Roboto-Regular.ttf", defaultFontSize, NULL, io.Fonts->GetGlyphRangesCyrillic());
    rlImGuiReloadFonts();
}

nano::UI::~UI()
{
    rlImGuiShutdown();
}

nano::UI& nano::UI::inst()
{
    static UI instance;
    return instance;
}

void nano::UI::processControls()
{
    /////// Input ///////
    if(::IsKeyPressed(KEY_TAB))
    {
        UI::inst().menuVisible = !UI::inst().menuVisible;
    }

    /////// flags ///////
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

void nano::UI::draw()
{
   
    processControls();

    rlImGuiBegin();
    {
        ImGui::PushFont(defaultFont);

        //ImGui::ShowDemoWindow();
        //ImGui::ShowStyleEditor();

        drawMainMenu();
        drawDropInstructions();
        drawExtremumAnalysisConfig();
        drawCalculatingProgressbar();
        drawConsole();
        drawAlertWindow();
        
        ImGui::PopFont();
    }
    rlImGuiEnd();
}

std::string nano::UI::floatToString(float f, uint8_t precision)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << f;
    return std::move(ss.str());
}

void nano::UI::print(std::string line)
{
    consoleBuffer.append(line.c_str());
    consoleScrollToBottom = true;
}

void nano::UI::printLine(std::string line)
{
    consoleBuffer.append(("\n" + line).c_str());
    consoleScrollToBottom = true;
}

void nano::UI::alert(std::string message)
{
    alertWindowVisible = true;
    alertText = std::move(message);
}

bool nano::UI::wantCaptureMouse() const
{
    return io->WantCaptureMouse;
}

void nano::UI::drawMainMenu()
{
    if(menuVisible)
    {
        ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        if(App::currImg.IsReady())
        {
            ImGui::Text(App::currImgPath.c_str());
            ImGui::Separator();
        }
        const float mainPanelHeight = 200.0f;
        ImGui::SetWindowPos({ 0.0f, (float)App::window.GetHeight() - mainPanelHeight});
        ImGui::SetWindowSize({ (float)App::window.GetWidth(), mainPanelHeight});
        if(App::currImg.IsReady() && ImGui::Button("Start extremum analysis")) 
        {
            extremumAnalysisConfigVisible = !extremumAnalysisConfigVisible;
            menuVisible = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Console")) consoleVisible = !consoleVisible;
        ImGui::SetTooltipD("show/hide console", 1.0f);
        if(App::maskTexture && ImGui::Button("Mask")) App::maskVisible = !App::maskVisible;
        ImGui::End();
    }
}

void nano::UI::drawDropInstructions()
{
    if(!App::currImg.IsReady())
    {
        static const char* text = "Drop an image to start analysis";
        static const uint8_t fontSize = 28u;
        static const Vector2 fontSizeVec = ::MeasureTextDefaultFont(text, fontSize);
        DrawText(text, (App::window.GetWidth() - fontSizeVec.x) * 0.5f, (App::window.GetHeight() - fontSizeVec.y) * 0.5, fontSize, WHITE);
    }
}

void nano::UI::drawExtremumAnalysisConfig()
{
    if(extremumAnalysisConfigVisible)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::Begin("Extremum analysis config", &extremumAnalysisConfigVisible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
        {
            ImGui::PushItemWidth(100.0f);
            if(App::currImg.IsReady())
            {
                ImGui::InputFloatClamped("Pixel size (nm)", &App::analyser.pixelSize_nm, 0.0f);
                ImGui::InputFloatClamped("Delta step", &App::analyser.extremumDeltaStep, 0.0f);
                if(App::analyser.processFullRange) ImGui::BeginDisabled();
                ImGui::InputIntClamped("Overflow tolerance", &App::analyser.extremumOverflowTolerance, 0);
                if(App::analyser.processFullRange) ImGui::EndDisabled();
                ImGui::Checkbox("Process full range", &App::analyser.processFullRange);
                ImGui::InputIntClamped("Min pixels in nanotube", &App::analyser.minPixelsInTube, 0);
            }
            else
            {
                ImGui::Text("No image to analyse!");
            }
            ImGui::PopItemWidth();
            if(ImGui::Button("Start"))
            {
                extremumAnalysisConfigVisible = false;
                App::startAnalysis();
            }
        }
        ImGui::End();
    }
}

void nano::UI::drawCalculatingProgressbar()
{
    if(App::calculating)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        //ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImGui::GetStyleColorVec4(ImGuiCol_TitleBgActive));
        //ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetStyleColorVec4(ImGuiCol_TitleBg));
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(center.x * 0.5f, 0.0f), ImGuiCond_Appearing);
        ImGui::Begin("Calculating...", NULL,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
        ImGui::ProgressBar(App::analyser.getProgress());
        //ImGui::PopStyleColor(2);
        ImGui::End();
    }
}

void nano::UI::drawConsole()
{
    if(consoleVisible)
    {
        ImGui::Begin("Console", &consoleVisible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar);

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
}

void nano::UI::drawAlertWindow()
{
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
}