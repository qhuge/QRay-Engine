#include "editor_popups_error.hpp"
#include "imgui.h"

// internal state (hidden from other files)
static bool gShowErrorPopup = false;
static std::string gErrorTitle;
static std::string gErrorMessage;

void ShowError(const std::string& title, const std::string& message)
{
    gErrorTitle = title;
    gErrorMessage = message;
    gShowErrorPopup = true;
}

void DrawErrorPopup()
{
    if (gShowErrorPopup)
    {
        ImGui::OpenPopup("ErrorPopup");
        gShowErrorPopup = false;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 0), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("ErrorPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "%s", gErrorTitle.c_str());

        ImGui::Separator();

        ImGui::TextWrapped("%s", gErrorMessage.c_str());

        ImGui::Spacing();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}