#ifndef IMGUI_WRAPPERS_HPP
#define IMGUI_WRAPPERS_HPP
 
#include <imgui.h>
#include <imgui_internal.h>
#include <limits>
#include <algorithm>


namespace ImGui
{

void SetTooltipD(const char* tip, float delay)
{
    if(IsItemHovered() && GImGui->HoveredIdTimer > delay)
    {
        SetTooltip(tip);
    }
}

bool InputIntClamped(const char* label, int* v, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max(),int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0)
{
    if(InputInt(label, v, step, step_fast, flags))
    {
        *v = std::clamp(*v, min, max);
        return true;
    }
    else
    {
        return false;
    }
}

bool InputFloatClamped(const char* label, float* v, float min = std::numeric_limits<float>::min(), float max = std::numeric_limits<int>::max(), float step = 0.0f, float step_fast =  0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
{
    if(InputFloat(label, v, step, step_fast, format, flags))
    {
        *v = std::clamp(*v, min, max);
        return true;
    }
    else
    {
        return false;
    }
}

} // namespace ImGui

#endif // IMGUI_WRAPPERS_HPP