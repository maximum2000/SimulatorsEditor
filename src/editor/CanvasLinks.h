#pragma once
#include "imgui.h"
#include <vector>

struct LinkSegment
{
    ImVec2 Start;
    ImVec2 End;
};

struct CanvasLink
{
    std::vector<LinkSegment> LinkSegments;
    bool isSelected = false;
    bool isSearched = false;
    bool isBlockSelection = false;
};