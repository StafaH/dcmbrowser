#include "ui.h"
#include "tinyfiledialogs/tinyfiledialogs.h"

void InitializeImGui(GLFWwindow *window, const char *glsl_version)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Disables caching of UI state to ini file
    io.IniFilename = NULL;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
}

void RenderImGui(GLFWwindow *window, UIState &state, std::vector<DicomPatient> &dicom_collection)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (state.no_titlebar)
        state.window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (state.no_scrollbar)
        state.window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!state.no_menu)
        state.window_flags |= ImGuiWindowFlags_MenuBar;
    if (state.no_move)
        state.window_flags |= ImGuiWindowFlags_NoMove;
    if (state.no_resize)
        state.window_flags |= ImGuiWindowFlags_NoResize;
    if (state.no_collapse)
        state.window_flags |= ImGuiWindowFlags_NoCollapse;
    if (state.no_nav)
        state.window_flags |= ImGuiWindowFlags_NoNav;
    if (state.no_background)
        state.window_flags |= ImGuiWindowFlags_NoBackground;
    if (state.no_bring_to_front)
        state.window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (state.no_close)
        state.open = NULL; // Don't pass our bool* to Begin

    // Main UI Interface
    ImGui::SetNextWindowPos(ImVec2(0, 20));
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    ImGui::SetNextWindowSize(ImVec2(width, height));
    if (ImGui::Begin("dcmbrowser", &state.open, state.window_flags))
    {
        RenderMenuBar(state);

        RenderBrowseAndScan(state, dicom_collection);

        RenderDicomFileTree(state, dicom_collection);

        ImGui::SameLine();

        RenderDicomFileInfo(state, dicom_collection);

        RenderExtraFeatures(state, dicom_collection);
    }
    ImGui::End();

    if (state.show_demo_window)
        ImGui::ShowDemoWindow(&state.show_demo_window);

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(state.clear_color.x, state.clear_color.y, state.clear_color.z, state.clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void CleanupImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void RenderMenuBar(UIState &state)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("About"))
            {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void RenderBrowseAndScan(UIState &state, std::vector<DicomPatient> &dicom_collection)
{
    // File Browsing
    ImGui::Text("Enter folder with DICOM files:");
    ImGui::SameLine();
    ImGui::InputText("", state.search_folder_path, IM_ARRAYSIZE(state.search_folder_path));
    ImGui::SameLine();
    if (ImGui::Button("...##searchdir"))
    {
        const char *folder_path = tinyfd_selectFolderDialog("Select a DICOM Folder", "C:/");
        if (!folder_path)
        { 
            /*Process Errors*/
        }
        strcpy_s(state.search_folder_path, folder_path);
    }
    ImGui::SameLine();
    if (ImGui::Button("Scan"))
    {
        LoadDicomFiles(state.search_folder_path, dicom_collection, state.scan_subdirectories);
    }

    // Options for directory browsing scanning
    ImGui::Checkbox("Scan subdirectories", &state.scan_subdirectories);
}

void RenderDicomFileTree(UIState &state, std::vector<DicomPatient> &dicom_collection)
{
    // File Display (Left)
    ImGui::BeginChild("Left Pane", ImVec2(300, 600), true);
    for (int i = 0; i < dicom_collection.size(); i++)
    {
        // Generate a tree node for each step of the heigharchy
        //ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        static int selection_mask = (1 << 2);
        int node_clicked = -1;
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3);

        if (ImGui::TreeNode(dicom_collection[i].patient_name.c_str()))
        {
            for (int j = 0; j < dicom_collection[i].dicom_studies.size(); j++)
            {
                if (ImGui::TreeNode(dicom_collection[i].dicom_studies[j].study_id.c_str()))
                {
                    for (int k = 0; k < dicom_collection[i].dicom_studies[j].dicom_series.size(); k++)
                    {
                        if (ImGui::TreeNode(dicom_collection[i].dicom_studies[j].dicom_series[k].series_id.c_str()))
                        {
                            for (int l = 0; l < dicom_collection[i].dicom_studies[j].dicom_series[k].dicom_files.size(); l++)
                            {
                                ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                                if (state.collection_index.DoesIndexMatch(i, j, k, l))
                                {
                                    node_flags |= ImGuiTreeNodeFlags_Selected;
                                }
                                // Generate a unique id for this item using the loop variables
                                int id = l + (10 * k) + (100 * j) + (1000 * i);
                                ImGui::TreeNodeEx((void *)(intptr_t)id, node_flags, "File %d", id);
                                if (ImGui::IsItemClicked())
                                {
                                    state.collection_index = CollectionIndex(i, j, k, l);
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        if (node_clicked != -1)
        {
            // Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
            if (ImGui::GetIO().KeyCtrl)
                selection_mask ^= (1 << node_clicked);
            else
                selection_mask = (1 << node_clicked);
        }
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
}

void RenderDicomFileInfo(UIState &state, std::vector<DicomPatient> &dicom_collection)
{
    // File Properties (Right)
    ImGui::BeginChild("Right Pane", ImVec2(800, 600), true);
    ImGui::Text("DICOM File Information;");

    ImGui::BeginTabBar("Properties Tab", ImGuiTabBarFlags_None);

    // Vectors of tags to be used for preview tab
    std::vector<DcmTagKey> important_tags =
        {DCM_PatientName, DCM_PatientID, DCM_StudyDate, DCM_StudyTime, DCM_StudyID, DCM_SeriesNumber,
         DCM_ReferringPhysicianName, DCM_PatientSex, DCM_PatientBirthDate, DCM_InstanceNumber};

    if (ImGui::BeginTabItem("Preview"))
    {
        if (!dicom_collection.empty())
        {
            for (int i = 0; i < important_tags.size(); i++)
            {
                const char* tag_string = LoadDicomTag(dicom_collection, state.collection_index, important_tags[i]); 
                ImGui::Text(tag_string);
            }
            
            // Render the image - Generates an OpenGL texture and renders using ImGui call
            RenderImageFromDicomFile(dicom_collection, state.collection_index);
        }
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Tags"))
    {
        DcmStack stack;
        DcmObject* obj = NULL;
        DcmElement* elem = NULL;
        
        DcmFileFormat file = GetDicomFileFromCollection(dicom_collection, state.collection_index);
        OFCondition iterator_status = file.getDataset()->nextObject(stack, OFTrue);

        while (iterator_status.good())
        {
            obj = stack.top();
            DcmEVR identifier = obj->ident();

            DcmTag tag = obj->getTag();
            const char* tag_name = tag.getTagName();
            
            elem = (DcmElement*)obj;
            OFString value;
            elem->getOFString(value, 0);

            ImGui::Text(tag_name);
            ImGui::SameLine();
            ImGui::Text(":  ");
            ImGui::SameLine();
            ImGui::Text(value.c_str());

            iterator_status = file.getDataset()->nextObject(stack, OFTrue);
        }

        ImGui::Text("Some Tags");
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
    ImGui::EndChild();
}

void RenderExtraFeatures(UIState &state, std::vector<DicomPatient> &dicom_collection)
{
    ImGui::Button("Anonymize");
    ImGui::SameLine();
    ImGui::Button("Organize Into Single Directory");
    ImGui::SameLine();
    ImGui::InputText("", state.organize_folder_path, IM_ARRAYSIZE(state.organize_folder_path));
    ImGui::SameLine();
    if (ImGui::Button("...##organizedir"))
    {
        const char *folder_path = tinyfd_selectFolderDialog("Select a DICOM Folder", "C:/");
        if (!folder_path)
        { /*Process Errors*/
        }
        strcpy_s(state.organize_folder_path, folder_path);
    }
}
