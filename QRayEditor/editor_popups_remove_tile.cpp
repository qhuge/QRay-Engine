#include "editor_popups_remove_tile.hpp"
#include "editor_helpers.hpp"
#include "imgui.h"
#include <cstring>
#include "editor_popups_error.hpp"
#include <algorithm>

// popup state
static bool gOpenPopup = false;

int indexToRemoveT;

void OpenRemoveTilePopup(int indexOfRemoved)
{
    indexToRemoveT = indexOfRemoved;
    gOpenPopup = true;
}

void DrawRemoveTilePopup()
{
    if (gOpenPopup)
    {
        ImGui::OpenPopup("Delete Tile Type");
        gOpenPopup = false;
    }

    if (ImGui::BeginPopupModal("Delete Tile Type", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Delete tile type named \"%s\"?", save.tileTypes[indexToRemoveT].name);

        ImGui::Text("(Used %d times)", save.tileTypes[indexToRemoveT].timesUsed);

        ImGui::Text("This action CANNOT be undone!");

        ImGui::Separator();

        //make the yes button red
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.15f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.20f, 0.20f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.55f, 0.10f, 0.10f, 1.0f));

        if (ImGui::Button("Yes"))
        {
            //save the path
            std::string asset = save.tileTypes[indexToRemoveT].texturePath;

            //go through all tiles and mark them to be deleted
            std::vector<int> indicesToDelete;
            for (int i = 0; i < save.tiles.size(); i++) {
                if (save.tiles[i].tileTypeIndex == indexToRemoveT) {

                    //if the tile is a door, then we must also delete the other part of the door
                    if (save.tileTypes[indexToRemoveT].isDoor) {
                        int otherDoor;
                        if (save.tiles[i].doorDirection == DoorDirection::Horizontal) {
                            otherDoor = FindWall(save.tiles[i].x + 1, save.tiles[i].y);
                        }
                        else if (save.tiles[i].doorDirection == DoorDirection::Vertical) {
                            otherDoor = FindWall(save.tiles[i].x, save.tiles[i].y + 1);
                        }

                        indicesToDelete.push_back(otherDoor);
                    }

                    indicesToDelete.push_back(i);
                }
            }

            std::sort(indicesToDelete.begin(), indicesToDelete.end(), std::greater<int>());

            //remove duplicate indices
            indicesToDelete.erase(std::unique(indicesToDelete.begin(), indicesToDelete.end()), indicesToDelete.end());

            //delete the tiles
            for (int index : indicesToDelete)
            {
                save.tiles.erase(save.tiles.begin() + index);
            }

            //update the indexes of other tiles
            for (auto& tile : save.tiles)
            {
                if (tile.tileTypeIndex > indexToRemoveT)
                {
                    tile.tileTypeIndex--;
                }
            }

            save.tileTypes.erase(save.tileTypes.begin() + indexToRemoveT);

            //delete the file
            bool success = DeleteAssetFile(saveLocation, asset);

            if (!success) {
                ShowError("Deleting texture file failed", "Couldn't delete the texture file. (" + asset + ")");
            }

            //then save
            SaveProject(saveLocation);

            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}