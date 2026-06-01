#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "editor_helpers.hpp"
#include <string>
#include "imgui.h"
#include "config.hpp"
#include <filesystem>

static std::string iniPath;


void DrawStartupWindow(HWND g_hWnd)
{
    static bool showNewProjectPopup = false;

    ImGui::SetNextWindowPos(
        ImGui::GetMainViewport()->GetCenter(),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f));

    ImGui::SetNextWindowSize(ImVec2(400, 200));

    ImGui::Begin(
        "Welcome to QRay",
        nullptr,
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove);

    if (ImGui::Button("New Project", ImVec2(200, 40)))
    {
        showNewProjectPopup = true;
    }

    if (ImGui::Button("Open Project", ImVec2(200, 40)))
    {
        std::string file = SelectProjectFile();

        if (!file.empty())
        {
            saveLocation = std::filesystem::path(file).parent_path().string();

            LoadProject(file, g_hWnd);

            gEditorState = EditorState::Editing;
        }
    }

    if (ImGui::Button("Exit", ImVec2(200, 40)))
    {
        PostQuitMessage(0);
    }

    ImGui::End();

    static char projectName[64] = "";
    static std::string projectLocation;

    if (showNewProjectPopup)
    {
        ImGui::OpenPopup("New Project");
        showNewProjectPopup = false;
    }

    if (ImGui::BeginPopupModal(
        "New Project",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Create a new project");
        ImGui::Separator();

        ImGui::Text("Project Name");
        ImGui::InputText(
            "##ProjectName",
            projectName,
            sizeof(projectName));

        ImGui::Spacing();

        ImGui::Text("Location");

        ImGui::SetNextItemWidth(350.0f);

        char locationBuffer[512];
        strcpy_s(
            locationBuffer,
            sizeof(locationBuffer),
            projectLocation.c_str());

        ImGui::InputText(
            "##Location",
            locationBuffer,
            sizeof(locationBuffer),
            ImGuiInputTextFlags_ReadOnly);

        ImGui::SameLine();

        if (ImGui::Button("Browse"))
        {
            std::string folder = SelectFolder();

            if (!folder.empty())
            {
                projectLocation = folder;
            }
        }

        ImGui::Spacing();
        ImGui::Separator();

        bool valid =
            strlen(projectName) > 0 &&
            !projectLocation.empty();

        if (!valid)
            ImGui::BeginDisabled();

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            CreateNewProject(projectLocation, projectName);

            std::string title = std::string("QRay Editor (") + save.projectTitle + ")";
            std::wstring wtitle(title.begin(), title.end());
            SetWindowTextW(g_hWnd, wtitle.c_str());

            iniPath = saveLocation + "/editor_layout.ini";

            ImGui::GetIO().IniFilename = iniPath.c_str();

            gEditorState = EditorState::Editing;

            ImGui::CloseCurrentPopup();
        }

        if (!valid)
            ImGui::EndDisabled();

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            projectName[0] = '\0';
            projectLocation.clear();

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}