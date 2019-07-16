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
        // Menu Bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open"))
                {
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // File Browsing
        ImGui::Text("Enter folder with DICOM files:");
        ImGui::SameLine();
        ImGui::InputText("", state.search_folder_path, IM_ARRAYSIZE(state.search_folder_path));
        ImGui::SameLine();
        if (ImGui::Button("...##searchdir"))
        {
            const char *folder_path = tinyfd_selectFolderDialog("Select a DICOM Folder", "C:/");
            if (!folder_path)
            { /*Process Errors*/
            }
            strcpy_s(state.search_folder_path, folder_path);
        }
        ImGui::SameLine();
        if (ImGui::Button("Scan"))
        {
            if (!state.scan_subdirectories)
            {
                for (auto &p : std::filesystem::directory_iterator(state.search_folder_path))
                {
                    if (p.path().extension() == ".dcm")
                    {
                        state.dicom_file_paths.emplace_back(p);

                        // Load all the files
                        DcmFileFormat fileformat;
                        OFCondition status = fileformat.loadFile(p.path().string().c_str());

                        if (!status.good())
                            std::cout << "error opening dicom file" << std::endl;

                        OFString patient_name;
                        OFString study_id;
                        OFString series_id;
                        status = fileformat.getDataset()->findAndGetOFString(DCM_PatientName, patient_name);
                        status = fileformat.getDataset()->findAndGetOFString(DCM_StudyID, study_id);
                        status = fileformat.getDataset()->findAndGetOFString(DCM_SeriesNumber, series_id);

                        if (!status.good())
                            std::cout << "error reading main DICOM tags" << std::endl;

                        if (dicom_collection.empty())
                        {
                            // Fill the collection with the first ever entry
                            DicomPatient new_patient;
                            new_patient.patient_name = patient_name;
                            DicomStudy new_study;
                            new_study.study_id = study_id;
                            DicomSeries new_series;
                            new_series.series_id = series_id;

                            new_series.dicom_files.push_back(fileformat);
                            new_study.dicom_series.push_back(new_series);
                            new_patient.dicom_studies.push_back(new_study);
                            dicom_collection.push_back(new_patient);
                        }
                        else
                        { // Check for previous entries
                            for (int i = 0; i < dicom_collection.size(); i++)
                            {
                                if (dicom_collection[i].patient_name == patient_name)
                                {
                                    for (int j = 0; j < dicom_collection[i].dicom_studies.size(); j++)
                                    {
                                        if (dicom_collection[i].dicom_studies[j].study_id == study_id)
                                        {
                                            for (int k = 0; k < dicom_collection[i].dicom_studies[j].dicom_series.size(); k++)
                                            {
                                                if (dicom_collection[i].dicom_studies[j].dicom_series[k].series_id == series_id)
                                                {
                                                    dicom_collection[i].dicom_studies[j].dicom_series[k].dicom_files.push_back(fileformat);
                                                }
                                                else
                                                { // Make a new Series
                                                    DicomSeries new_series;
                                                    new_series.series_id = series_id;
                                                    new_series.dicom_files.push_back(fileformat);
                                                    dicom_collection[i].dicom_studies[j].dicom_series.push_back(new_series);
                                                }
                                            }
                                        }
                                        else
                                        { // Make a new Study
                                            DicomStudy new_study;
                                            new_study.study_id = study_id;
                                            DicomSeries new_series;
                                            new_series.series_id = series_id;
                                            new_series.dicom_files.push_back(fileformat);
                                            dicom_collection[i].dicom_studies.push_back(new_study);
                                        }
                                    }
                                }
                                else
                                { // Make a new patient group
                                    DicomPatient new_patient;
                                    new_patient.patient_name = patient_name;
                                    DicomStudy new_study;
                                    new_study.study_id = study_id;
                                    DicomSeries new_series;
                                    new_series.series_id = series_id;

                                    new_series.dicom_files.push_back(fileformat);
                                    new_study.dicom_series.push_back(new_series);
                                    new_patient.dicom_studies.push_back(new_study);
                                    dicom_collection.push_back(new_patient);
                                }
                            }
                        }
                    }
                }

                // End of scan
                DebugDicomFileScan(dicom_collection);
            }
            else
            {
                for (auto &p : std::filesystem::recursive_directory_iterator(state.search_folder_path))
                {
                    if (p.path().extension() == ".dcm")
                    {
                    }
                }
            }
        }

        // Options for directory browsing scanning
        ImGui::Checkbox("Scan subdirectories", &state.scan_subdirectories);

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
                                    if (state.patient_index == i && state.study_index == j && state.series_index == k && state.file_index == l)
                                    {
                                        node_flags |= ImGuiTreeNodeFlags_Selected;
                                    }
                                    // Generate a unique id for this item using the loop variables
                                    int id = l + (10 * k) + (100 * j) + (1000 * i);
                                    ImGui::TreeNodeEx((void *)(intptr_t)id, node_flags, "File %d", id);
                                    if (ImGui::IsItemClicked())
                                    {
                                        state.patient_index = i;
                                        state.study_index = j;
                                        state.series_index = k;
                                        state.file_index = l;

                                        //node_clicked = i;
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
            //if (ImGui::Selectable((char *)state.dicom_file_paths[i].filename().string().c_str(), state.selected_dicom_file == i))
            // state.selected_dicom_file = i;
        }
        ImGui::EndChild();
        ImGui::SameLine();

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
                // Retrive the information currently indexed(selected) file
                DcmFileFormat file = dicom_collection[state.patient_index].dicom_studies[state.study_index].dicom_series[state.series_index].dicom_files[state.file_index];
                for (int i = 0; i < important_tags.size(); i++)
                {
                    OFString value;
                    OFCondition status = file.getDataset()->findAndGetOFString(important_tags[i], value);
                    if (!status.good()) value = "Unable to find this tag";
                    ImGui::Text(value.c_str());
                }
            }
            ImGui::EndTabItem();

            if (ImGui::BeginTabItem("Tags"))
            {
                ImGui::Text("Some Tags");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndChild();

        // File Manipulation Options
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

/* Iterate over the dicom_collection nested vectors to debug how many files were found */
void DebugDicomFileScan(const std::vector<DicomPatient> dicom_collection)
{
    std::cout << "Found " << dicom_collection.size() << " patients" << std::endl;

    for (int i = 0; i < dicom_collection.size(); i++)
    {
        std::cout << "      ";
        std::cout << "Patient " << i << " has " << dicom_collection[i].dicom_studies.size() << " studies" << std::endl;

        for (int j = 0; j < dicom_collection[i].dicom_studies.size(); j++)
        {
            std::cout << "              ";
            std::cout << "Study " << j << " has " << dicom_collection[i].dicom_studies[j].dicom_series.size() << " series" << std::endl;

            for (int k = 0; k < dicom_collection[i].dicom_studies[j].dicom_series.size(); k++)
            {
                std::cout << "                      ";
                std::cout << "Series " << k << " has " << dicom_collection[i].dicom_studies[j].dicom_series[k].dicom_files.size() << " files" << std::endl;
            }
        }
    }
}