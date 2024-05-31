#pragma once
#include "raylib/raylib.h"

struct Window {
    Vector2 minSize;
    Rectangle rect;
    Color color;
    bool isDragging;
    bool isResizing;
    Vector2 dragOffset;
    Vector2 resizeOffset;
};
