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
    bool isSelected = false;
    bool isBlockSelection = false;
};