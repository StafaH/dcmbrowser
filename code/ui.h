#pragma once

#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

struct UIState
{
    bool open = true;
    bool show_demo_window = false;
    ImVec4 clear_color = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);

    // Window flags
    ImGuiWindowFlags window_flags = 0;
    bool no_titlebar = true;
    bool no_scrollbar = false;
    bool no_menu = true;
    bool no_move = false;
    bool no_resize = false;
    bool no_collapse = false;
    bool no_close = false;
    bool no_nav = false;
    bool no_background = true;
    bool no_bring_to_front = false;
};

// UI main class
void InitializeImGui(GLFWwindow* window, const char* glsl_version);

void RenderImGui(GLFWwindow* window, UIState state);

void CleanupImGui();

