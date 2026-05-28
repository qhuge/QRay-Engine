#include "editor_window.hpp"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    EditorState editor = {};

    if (!InitEditorWindow(
        editor,
        hInstance,
        nCmdShow))
    {
        return -1;
    }

    MSG msg = {};

    while (GetMessage(
        &msg,
        nullptr,
        0,
        0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}