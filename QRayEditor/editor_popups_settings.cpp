#include "editor_popups_settings.hpp"
#include "imgui.h"
#include "config.hpp"
#include <string>
#include <cstring>
#include <cmath>

// internal popup state
static bool gShowSettingsPopup = false;

// temporary buffers
static char gGameName[64];
static char gProjectName[64];

static int gWindowWidth;
static int gWindowHeight;
static int gRenderDistance;
static float gStartingAngle;
static float gCeilingColor[3];
static float gFloorColor[3];

// --------------------------------------------------
// Public API
// --------------------------------------------------

void OpenSettingsPopup()
{
    strcpy_s(gGameName, save.gameTitle);
    strcpy_s(gProjectName, save.projectTitle);

    gWindowWidth = save.resolutionX;
    gWindowHeight = save.resolutionY;
    gRenderDistance = save.renderDistance;
    gStartingAngle = save.startingAngle + 90.0f;

    gCeilingColor[0] = save.ceilingColor[0];
    gCeilingColor[1] = save.ceilingColor[1];
    gCeilingColor[2] = save.ceilingColor[2];

    gFloorColor[0] = save.floorColor[0];
    gFloorColor[1] = save.floorColor[1];
    gFloorColor[2] = save.floorColor[2];

    gShowSettingsPopup = true;
}

void DrawSettingsPopup()
{
    if (gShowSettingsPopup)
    {
        ImGui::OpenPopup("Project Settings");
        gShowSettingsPopup = false;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 0), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Project Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Game Name");
        ImGui::InputText("##gameName", gGameName, sizeof(gGameName));

        ImGui::Text("Project Name");
        ImGui::InputText("##projectName", gProjectName, sizeof(gProjectName));

        ImGui::Separator();

        ImGui::Text("Window Width");
        ImGui::InputInt("##width", &gWindowWidth);

        ImGui::Text("Window Height");
        ImGui::InputInt("##height", &gWindowHeight);

        ImGui::Text("Render Distance");
        ImGui::InputInt("##renderDistance", &gRenderDistance);

        ImGui::Text("Starting Angle");
        ImGui::SliderFloat("##angle", &gStartingAngle, 0.0f, 360.0f);

        ImGui::Text("Ceiling Color");
        ImGui::ColorEdit3("##ceilingColor", gCeilingColor);

        ImGui::Text("Floor Color");
        ImGui::ColorEdit3("##floorColor", gFloorColor);

        ImGui::Separator();

        float buttonWidth = 120.0f;
        float spacing = 10.0f;
        float totalWidth = buttonWidth * 2 + spacing;

        float avail = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail - totalWidth) * 0.5f);

        if (ImGui::Button("Apply", ImVec2(buttonWidth, 0)))
        {
            // apply to config
            strcpy_s(save.gameTitle, gGameName);

            save.resolutionX = gWindowWidth;
            save.resolutionY = gWindowHeight;
            save.renderDistance = gRenderDistance;
            save.startingAngle = gStartingAngle - 90.0f;

            save.ceilingColor[0] = gCeilingColor[0];
            save.ceilingColor[1] = gCeilingColor[1];
            save.ceilingColor[2] = gCeilingColor[2];

            save.floorColor[0] = gFloorColor[0];
            save.floorColor[1] = gFloorColor[1];
            save.floorColor[2] = gFloorColor[2];

            strcpy_s(save.projectTitle, gProjectName);

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine(0, spacing);

        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}