#include "editor_popups_add_tile.hpp"
#include "imgui.h"
#include "world.hpp"
#include "config.hpp"
#include <cstring>
#include "editor_helpers.hpp"

// popup state
static bool gOpenPopup = false;

static char gTileName[64] = "New Tile";
static std::string gPendingTexture;
static bool gPendingDoor = false;

void OpenAddTilePopup()
{
    gPendingDoor = false;
    gOpenPopup = true;
    strcpy_s(gTileName, "New Tile");
    gPendingTexture.clear();
}

void DrawAddTilePopup()
{
    if (gOpenPopup)
    {
        ImGui::OpenPopup("Add Tile Type");
        gOpenPopup = false;
    }

    ImGui::SetNextWindowSize(ImVec2(600, 250), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Add Tile Type", nullptr, ImGuiWindowFlags_NoResize))
    {
        ImGui::Text("Tile Name:");
        ImGui::InputText("##tile", gTileName, 64);

        ImGui::Spacing();

        ImGui::Text("Texture:");

        ImGui::TextWrapped(gPendingTexture.c_str());

        if (ImGui::Button("Browse PNG"))
        {
            gPendingTexture = OpenFileDialogPNG();
        }

        ImGui::Checkbox("Door", &gPendingDoor);

        ImGui::Separator();

        float footerY = ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() - 10.0f;
        ImGui::SetCursorPosY(footerY);

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            if (strlen(gTileName) > 0 && !gPendingTexture.empty())
            {
                std::string relative;

                if (CopyAssetToProject(gPendingTexture, saveLocation, relative))
                {
                    TileType t;
                    strcpy_s(t.name, gTileName);
                    strcpy_s(t.texturePath, relative.c_str());
                    t.isDoor = gPendingDoor;

                    SetRandomColorForTile(t);

                    save.tileTypes.push_back(t);

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