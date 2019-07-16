#pragma once

#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "dcmtk/dcmdata/dctk.h"
#include <filesystem>
#include <vector>

/*
Dicom Grouping Hierarchy
- Patient Name
    - Study ID 
        - Series ID
*/ 
struct DicomSeries
{
    OFString series_id;
    std::vector<DcmFileFormat> dicom_files;
};

struct DicomStudy
{
    OFString study_id;
    std::vector<DicomSeries> dicom_series;
};

struct DicomPatient
{
    OFString patient_name;
    std::vector<DicomStudy> dicom_studies;

};

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
    bool no_move = true;
    bool no_resize = true;
    bool no_collapse = true;
    bool no_close = false;
    bool no_nav = false;
    bool no_background = true;
    bool no_bring_to_front = false;

    // UI State Variables
    char search_folder_path[255];
    bool scan_subdirectories = false;
    std::vector<std::filesystem::path> dicom_file_paths;
    int selected_dicom_file = 0;
    char organize_folder_path[255] = "C:/DICOM/";
};

// UI main class
void InitializeImGui(GLFWwindow* window, const char* glsl_version);

void RenderImGui(GLFWwindow* window, UIState& state, std::vector<DicomPatient>& dicom_collection);

void CleanupImGui();

void DebugDicomFileScan(const std::vector<DicomPatient> dicom_collection);

