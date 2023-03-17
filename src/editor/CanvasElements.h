#pragma once
#include "imgui.h"

struct CanvasElement
{
    int elementDataNumber;
    short elementDataType;
    ImVec2 position;
    ImVec2 centerPosition;
    bool isSelected = false;
    bool isSearched = false;
    bool isBlockSelection = false;
};