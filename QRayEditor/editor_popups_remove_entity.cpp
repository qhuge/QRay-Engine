#include "editor_popups_remove_entity.hpp"
#include "editor_helpers.hpp"
#include "imgui.h"
#include <cstring>
#include "editor_popups_error.hpp"

// popup state
static bool gOpenPopup = false;

int indexToRemoveE;

void OpenRemoveEntityPopup(int indexOfRemoved)
{
    indexToRemoveE = indexOfRemoved;
    gOpenPopup = true;
}

void DrawRemoveEntityPopup()
{
    if (gOpenPopup)
    {
        ImGui::OpenPopup("Delete Entity Type");
        gOpenPopup = false;
    }

    if (ImGui::BeginPopupModal("Delete Entity Type", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Delete entity type named \"%s\"?", save.entityTypes[indexToRemoveE].name);

        ImGui::Text("(Used %d times)", save.entityTypes[indexToRemoveE].timesUsed);

        ImGui::Text("This action CANNOT be undone!");

        ImGui::Separator();

        //make the yes button red
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.15f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.20f, 0.20f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.55f, 0.10f, 0.10f, 1.0f));

        if (ImGui::Button("Yes"))
        {
            //save the path
            std::string asset = save.entityTypes[indexToRemoveE].texturePath;

            //go through all entities and remove them from the map
            for (int i = save.entities.size() - 1; i >= 0; i--) {
                if (save.entities[i].entityTypeIndex == indexToRemoveE) {
                    save.entities.erase(save.entities.begin() + i);
                }
            }

            //update the indexes of other entites
            for (auto& entity : save.entities)
            {
                if (entity.entityTypeIndex > indexToRemoveE)
                {
                    entity.entityTypeIndex--;
                }
            }

            save.entityTypes.erase(save.entityTypes.begin() + indexToRemoveE);

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