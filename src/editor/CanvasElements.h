#pragma once
#include "imgui.h"

struct CanvasElement
{
    int elementDataNumber;
    ImVec2 position;
    ImVec2 centerPosition;
    bool isSelected = false;
    bool isBlockSelection = false;
};