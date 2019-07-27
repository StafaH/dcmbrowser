// Include ImGui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// Standard Libraries
#include <stdio.h>
#include <iostream>

// Window/Rendering Libraries
#include <GL/glew.h> // Initialize with glewInit()
#include <GLFW/glfw3.h>

//dcmtk
#include <dcmtk/dcmdata/dctk.h>

// Local Includes
#include "ui.h"
#include "browser.h"

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char **)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(1280, 720, "dcmbrowser", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = glewInit() != GLEW_OK;

    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    InitializeImGui(window, glsl_version);

    UIState uiState;
    std::vector<DicomPatient> dicom_collection;
    
    // Main loop
    while (!glfwWindowShouldClose(window))
    {

        glfwPollEvents();

        RenderImGui(window, uiState, dicom_collection);
        glfwSwapBuffers(window);
    }

    CleanupImGui();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}