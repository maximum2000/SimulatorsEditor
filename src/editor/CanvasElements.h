#pragma once
#include "imgui.h"
#include <string>
#include <vector>

struct CanvasElement
{
    std::string UUID;
    int elementDataNumber;
    short elementDataType;
    ImVec2 position;
    ImVec2 centerPosition;
    std::vector<std::string> Links;
    bool isSelected = false;
    bool isSearched = false;
    bool isBlockSelection = false;
};