#pragma once
#include "imgui.h"
#include <vector>
#include <string>
#include "CanvasElements.h"


struct CanvasLink
{
    std::string UUID;
    std::vector<ImVec2> linkDots;
    std::string firstElementUUID;
    std::string secondElementUUID;
    std::vector<std::string> Elements;
    bool isSelected = false;
    bool isBlockSelection = false;
};