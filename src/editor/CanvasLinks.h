#pragma once
#include "imgui.h"
#include <vector>
#include "CanvasElements.h"


struct CanvasLink
{
    std::vector<ImVec2> linkDots;
    CanvasElement* first;
    CanvasElement* second;
    bool isSelected = false;
    bool isBlockSelection = false;
};