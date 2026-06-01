#include "editor_popups_add_entity.hpp"
#include "imgui.h"
#include "world.hpp"
#include "config.hpp"
#include <cstring>
#include "editor_helpers.hpp"

// popup state
static bool gOpenPopup = false;

static char gEntityName[64] = "New Entity";
static std::string gPendingTexture;
static int gHealth = 100;
static uint32_t gEntityTag = 0;
static int gIdleMovement = IDLE_NONE;
static int gLOSMovement = LOS_NONE;
static bool gEntityStatic = true;

void OpenAddEntityPopup()
{
    gOpenPopup = true;
    strcpy_s(gEntityName, "New Entity");
    gEntityStatic = true;
    gLOSMovement = LOS_NONE;
    gIdleMovement = IDLE_NONE;
    gEntityTag = 0;
    gHealth = 100;
    gPendingTexture.clear();
}

void DrawAddEntityPopup()
{
    if (gOpenPopup)
    {
        ImGui::OpenPopup("Add Entity Type");
        gOpenPopup = false;
    }

    ImGui::SetNextWindowSize(ImVec2(600, 380), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Add Entity Type", nullptr, ImGuiWindowFlags_NoResize))
    {
        ImGui::Text("Entity Name:");
        ImGui::InputText("##entity", gEntityName, 64);

        ImGui::Spacing();

        ImGui::Text("Texture:");

        ImGui::TextWrapped(gPendingTexture.c_str());

        if (ImGui::Button("Browse PNG"))
        {
            gPendingTexture = OpenFileDialogPNG();
        }

        ImGui::Separator();

        int tag = (int)gEntityTag;

        ImGui::Text("Tag:");
        ImGui::InputInt("##tag", &tag);

        if (tag < 0)
            tag = 0;

        gEntityTag = (uint32_t)tag;

        ImGui::Spacing();

        ImGui::Checkbox("Static Entity", &gEntityStatic);

        // Only show these for non-static entities
        if (!gEntityStatic)
        {
            ImGui::Spacing();

            const char* idleMovementNames[] =
            {
                "None",
                "Random Walk"
            };

            ImGui::Text("Idle Movement");
            ImGui::Combo(
                "##idlemovement",
                &gIdleMovement,
                idleMovementNames,
                IM_ARRAYSIZE(idleMovementNames)
            );

            const char* losMovementNames[] =
            {
                "None",
                "Shoot Player",
                "Charge Player"
            };

            ImGui::Text("LOS Movement");
            ImGui::Combo(
                "##losmovement",
                &gLOSMovement,
                losMovementNames,
                IM_ARRAYSIZE(losMovementNames)
            );

            ImGui::Text("Health");
            ImGui::DragInt("##health", &gHealth, 1, 1, 100000);
        }

        ImGui::Separator();

        float footerY = ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() - 10.0f;
        ImGui::SetCursorPosY(footerY);

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            if (strlen(gEntityName) > 0 && !gPendingTexture.empty())
            {
                std::string relative;

                if (CopyAssetToProject(gPendingTexture, saveLocation, relative))
                {
                    EntityType e;
                    strcpy_s(e.name, gEntityName);
                    strcpy_s(e.texturePath, relative.c_str());
                    e.tag = gEntityTag;

                    if (gEntityStatic)
                    {
                        e.health = -1;
                        e.idleMovement = IDLE_NONE;
                        e.losMovement = LOS_NONE;
                    }
                    else
                    {
                        e.health = gHealth;
                        e.idleMovement = (IdleMovementType)gIdleMovement;
                        e.losMovement = (LOSMovementType)gLOSMovement;
                    }

                    SetRandomColorForEntity(e);

                    save.entityTypes.push_back(e);

                    SaveProject(saveLocation);
                }
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}