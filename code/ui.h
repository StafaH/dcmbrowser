#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "dcmtk/dcmdata/dctk.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include <vector>
#include <string>
#include "browser.h"
#include <GLFW/glfw3.h>

struct UIState
{    
public: 
    bool open = true;
    ImVec4 clear_color = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);

    // Window flags
    ImGuiWindowFlags window_flags = 0;
    bool no_titlebar = true;
    bool no_scrollbar = false;
    bool no_menu = true;
    bool no_move = true;
    bool no_resize = true;
    bool no_collapse = true;
    bool no_close = false;
    bool no_nav = false;
    bool no_background = true;
    bool no_bring_to_front = false;

    // Directory Search Variables
    char search_folder_path[255];
    bool scan_subdirectories = false;
    
    // Dicom Tree variables
    CollectionIndex collection_index = CollectionIndex();

    // Dicom Extra Features
    char organize_folder_path[255] = "C:/DICOM/";

    // Debug
    bool show_demo_window = false;
};

// UI main class
void InitializeImGui(GLFWwindow* window, const char* glsl_version);

void RenderImGui(GLFWwindow* window, UIState& state, std::vector<DicomPatient>& dicom_collection);

void RenderMenuBar(GLFWwindow *window, UIState& state);

void RenderBrowseAndScan(UIState& state, std::vector<DicomPatient>& dicom_collection);

void RenderDicomFileTree(UIState& state, std::vector<DicomPatient>& dicom_collection);

void RenderDicomFileInfo(UIState& state, std::vector<DicomPatient>& dicom_collection);

void RenderExtraFeatures(UIState& state, std::vector<DicomPatient>& dicom_collection);

void CleanupImGui();

