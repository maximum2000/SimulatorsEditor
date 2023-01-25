#pragma once
#include "imgui.h"

struct CanvasElement
{
    int elementDataNumber;
    ImVec2 position;
    bool isSelected = false;
};