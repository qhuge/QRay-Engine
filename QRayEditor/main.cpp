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


    /*Tile t;
    t.x = 50;
    t.y = 50;
    t.tileTypeIndex = 0;
    worldTiles.push_back(t);

    Entity e;
    e.x = 51;
    e.y = 50;
    e.entityTypeIndex = 0;
    worldEntities.push_back(e);*/

    InitEditor(hInstance, nCmdShow);
    RunEditorLoop();

    CoUninitialize();
    ShutdownEditor();
    return 0;
}