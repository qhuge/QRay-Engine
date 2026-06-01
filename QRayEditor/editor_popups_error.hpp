#pragma once
#include <string>

// trigger error popup
void ShowError(const std::string& title, const std::string& message);

// must be called every frame
void DrawErrorPopup();