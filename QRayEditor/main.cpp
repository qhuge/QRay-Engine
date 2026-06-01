#include <windows.h>
#include "editor_main.hpp"
#include "world.hpp"
#include <string>
#include "editor_helpers.hpp"
#include <chrono>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    srand(time(0));

    InitEditor(hInstance, nCmdShow);
    RunEditorLoop();

    CoUninitialize();
    ShutdownEditor();
    return 0;
}