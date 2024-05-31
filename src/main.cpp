#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <numeric>

#include "raylib/raylib.h"
#include "window.h"

extern "C"
{
    #include <lua5.4/lua.h>
    #include <lua5.4/lauxlib.h>
    #include <lua5.4/lualib.h>
}

namespace fs = std::filesystem;

#define TARGET_FPS 240
#define WINDOW_TITLE "VRNI-WM"
#define MIN_WINDOW_WIDTH 50.0f
#define MIN_WINDOW_HEIGHT 50.0f
#define MOD_PATH "data/mods"

bool CheckLua(lua_State *L, int r)
{
    if (r != LUA_OK)
    {
        std::string errormsg = lua_tostring(L, -1);
        std::cerr << "[ERROR/LUA]: " << errormsg << std::endl;
        return false;
    }
    return true;
}

bool init_check()
{
        lua_State *L = luaL_newstate();
        luaL_openlibs(L);

        if (CheckLua(L, luaL_dofile(L, "data/scripts/test.lua")))
        {
            lua_getglobal(L, "a");
            if (lua_isnumber(L, -1))
            {
                float result = (float)lua_tonumber(L, -1);
                if (result != -0.750987)
               {
                   lua_close(L);
                   return true;
               }
               std::cerr << "[ERROR/LUA]: Failed the initial Check: Different Test Result! (" << result << ")" << std::endl;
               lua_close(L);
               return false;
            }
        }
        lua_close(L);
        return false;
}

std::vector<std::string> getMods() {
    std::vector<std::string> Mods;

    try {
        for(const auto& entry : fs::directory_iterator(MOD_PATH)) {
            Mods.push_back(entry.path());
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "[ERROR/FILESYS]: " << e.what() << std::endl;
        exit(1);
    }

    return Mods;
}

std::vector<Window> hotReloadWindows(std::vector<std::string> Mods) {
    std::vector<Window> windows = {};

    for (const std::string& mod : Mods)
    {
        int loc_minWidth = 0;
        int loc_minHeight = 0;
        int loc_starting_pos_x = 0;
        int loc_starting_pos_y = 0;
        int loc_startingWidth = 0;
        int loc_startingHeight = 0;
        int loc_r = 0;
        int loc_g = 0;
        int loc_b = 0;
        int loc_a = 0;

        lua_State *L = luaL_newstate();
        luaL_openlibs(L);
        std::string window_script_path = mod + "/" + "window.lua";
        if (CheckLua(L, luaL_dofile(L, window_script_path.c_str())))
        {
            lua_getglobal(L, "minWidth");
            if (lua_isnumber(L, -1)){loc_minWidth = (int)lua_tonumber(L, -1);}
            lua_settop(L, 0);

            lua_getglobal(L, "minHeight");
            if (lua_isnumber(L, -1)){loc_minHeight = (int)lua_tonumber(L, -1);}
            lua_settop(L, 0);

            lua_getglobal(L, "starting_pos_x");
            if (lua_isnumber(L, -1)){loc_starting_pos_x = (int)lua_tonumber(L, -1);}
            lua_settop(L, 0);

            lua_getglobal(L, "starting_pos_y");
            if (lua_isnumber(L, -1)){loc_starting_pos_y = (int)lua_tonumber(L, -1);}
            lua_settop(L, 0);

            lua_getglobal(L, "startingWidth");
            if (lua_isnumber(L, -1)){loc_startingWidth = (int)lua_tonumber(L, -1);}
            lua_settop(L, 0);

            lua_getglobal(L, "startingHeight");
            if (lua_isnumber(L, -1)){loc_startingHeight = (int)lua_tonumber(L, -1);}
            lua_settop(L, 0);

            lua_getglobal(L, "window_color_r");
            if (lua_isnumber(L, -1)){loc_r = (int)lua_tonumber(L, -1);}
            lua_settop(L, 0);

            lua_getglobal(L, "window_color_g");
            if (lua_isnumber(L, -1)){loc_g = (int)lua_tonumber(L, -1);}
            lua_settop(L, 0);

            lua_getglobal(L, "window_color_b");
            if (lua_isnumber(L, -1)){loc_b = (int)lua_tonumber(L, -1);}
            lua_settop(L, 0);

            lua_getglobal(L, "window_color_a");
            if (lua_isnumber(L, -1)){loc_a = (int)lua_tonumber(L, -1);}
            lua_settop(L, 0);

        }

        lua_close(L);

        windows.push_back({{loc_minWidth, loc_minHeight}, {loc_starting_pos_x, loc_starting_pos_y, loc_startingWidth, loc_startingHeight}, {loc_r, loc_g, loc_b, loc_a}, false, false, {0, 0}, {0, 0}});
    }

    return windows;
}

std::vector<int> reset_zOrder(int windowCount) {
    std::vector<int> zOrder(windowCount);
    std::iota (std::begin(zOrder), std::end(zOrder), 0);
    return zOrder;
}

int main()
{
    if (!init_check())
    {
        return -1;
    }

    std::vector<std::string> Mods = getMods();
    std::vector<Window> windows = hotReloadWindows(Mods);
    int windowCount = windows.size();
    std::vector<int> zOrder = reset_zOrder(windowCount);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(800, 600, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    Window* activeWindow = nullptr;
    //---BEGIN-RAYLIB-LOOP---

    while (!WindowShouldClose()) {
        if(IsKeyPressed(KEY_R)) {
            Mods = getMods();
            windows = hotReloadWindows(Mods);
            windowCount = windows.size();
            zOrder = reset_zOrder(windowCount);
        }

        int counter = 0;
        for (const std::string& mod : Mods)
        {
            int loc_minWidth = 0;
            int loc_minHeight = 0;
            int loc_r = 0;
            int loc_g = 0;
            int loc_b = 0;
            int loc_a = 0;

            lua_State *L = luaL_newstate();
            luaL_openlibs(L);
            std::string window_script_path = mod + "/" + "window.lua";
            if (CheckLua(L, luaL_dofile(L, window_script_path.c_str())))
            {
                lua_getglobal(L, "minWidth");
                if (lua_isnumber(L, -1)){loc_minWidth = (int)lua_tonumber(L, -1);}
                lua_settop(L, 0);

                lua_getglobal(L, "minHeight");
                if (lua_isnumber(L, -1)){loc_minHeight = (int)lua_tonumber(L, -1);}
                lua_settop(L, 0);

                lua_getglobal(L, "window_color_r");
                if (lua_isnumber(L, -1)){loc_r = (int)lua_tonumber(L, -1);}
                lua_settop(L, 0);

                lua_getglobal(L, "window_color_g");
                if (lua_isnumber(L, -1)){loc_g = (int)lua_tonumber(L, -1);}
                lua_settop(L, 0);

                lua_getglobal(L, "window_color_b");
                if (lua_isnumber(L, -1)){loc_b = (int)lua_tonumber(L, -1);}
                lua_settop(L, 0);

                lua_getglobal(L, "window_color_a");
                if (lua_isnumber(L, -1)){loc_a = (int)lua_tonumber(L, -1);}
                lua_settop(L, 0);

            }

            lua_close(L);
            Vector2 updated_minSize = {loc_minWidth, loc_minHeight};
            Color updated_color = {loc_r, loc_g, loc_b, loc_a};
            Window updated_window = {updated_minSize, windows[counter].rect, updated_color, windows[counter].isDragging, windows[counter].isResizing, windows[counter].dragOffset, windows[counter].resizeOffset};
            windows[counter] = updated_window;
            counter++;
        }

        Vector2 mousePosition = GetMousePosition();
        bool mouseLeftPressed = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        bool mouseLeftReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

        if (activeWindow) {
            if (activeWindow->isDragging) {
                activeWindow->rect.x = mousePosition.x - activeWindow->dragOffset.x;
                activeWindow->rect.y = mousePosition.y - activeWindow->dragOffset.y;
                if (mouseLeftReleased) activeWindow->isDragging = false;
            } else if (activeWindow->isResizing) {
                activeWindow->rect.width = mousePosition.x - activeWindow->rect.x + activeWindow->resizeOffset.x;
                activeWindow->rect.height = mousePosition.y - activeWindow->rect.y + activeWindow->resizeOffset.y;
                if (mouseLeftReleased) activeWindow->isResizing = false;

                // Ensure the window size is not negative
                if (activeWindow->rect.width < activeWindow->minSize.x) activeWindow->rect.width = activeWindow->minSize.x;
                if (activeWindow->rect.height < activeWindow->minSize.y) activeWindow->rect.height = activeWindow->minSize.y;

                if (mouseLeftReleased) activeWindow->isResizing = false;
            }

            if (!activeWindow->isDragging && !activeWindow->isResizing) {
                activeWindow = nullptr;
            }
        } else if (mouseLeftPressed) {
            for (int i = 0; i < windows.size(); ++i) {
                if (CheckCollisionPointRec(mousePosition, windows[i].rect)) {
                    activeWindow = &windows[i];
                    if (mousePosition.x > windows[i].rect.x + windows[i].rect.width - 10 && mousePosition.y > windows[i].rect.y + windows[i].rect.height - 10) {
                        // Start resizing
                        activeWindow->isResizing = true;
                        activeWindow->resizeOffset = {windows[i].rect.width - (mousePosition.x - windows[i].rect.x), windows[i].rect.height - (mousePosition.y - windows[i].rect.y)};
                    } else {
                        // Start dragging
                        activeWindow->isDragging = true;
                        activeWindow->dragOffset = {mousePosition.x - windows[i].rect.x, mousePosition.y - windows[i].rect.y};
                    }
                    // Move the interacted window's index to the end of the zOrder list to bring it to the foreground
                    auto it = std::find(zOrder.begin(), zOrder.end(), i);
                    if (it != zOrder.end()) {
                        zOrder.erase(it);
                        zOrder.push_back(i);
                    }
                    break;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10, 10);

        // Draw windows in the order defined by zOrder
        for (int i = 0; i < zOrder.size(); ++i) {
            int index = zOrder[i];
            DrawRectangleRec(windows[index].rect, windows[index].color);
            DrawRectangleLinesEx(windows[index].rect, 1, BLACK);
        }
        EndDrawing();
    }

    CloseWindow();

    //---END-RAYLIB-LOOP---
    return 0;
}
