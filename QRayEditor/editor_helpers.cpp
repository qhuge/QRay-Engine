#include <windows.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include "editor_helpers.hpp"
#include <iostream>
#include <fstream>

std::string SelectFolder()
{
    IFileDialog* pfd = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_FileOpenDialog,
        nullptr,
        CLSCTX_ALL,
        IID_PPV_ARGS(&pfd));

    if (FAILED(hr))
    {
        return "";
    }

    DWORD options;

    pfd->GetOptions(&options);

    pfd->SetOptions(
        options | FOS_PICKFOLDERS);

    hr = pfd->Show(nullptr);

    if (FAILED(hr))
    {
        pfd->Release();
        return "";
    }

    IShellItem* psi = nullptr;

    hr = pfd->GetResult(&psi);

    if (FAILED(hr))
    {
        pfd->Release();
        return "";
    }

    PWSTR widePath = nullptr;

    hr = psi->GetDisplayName(
        SIGDN_FILESYSPATH,
        &widePath);

    if (FAILED(hr))
    {
        psi->Release();
        pfd->Release();
        return "";
    }

    char path[MAX_PATH];

    WideCharToMultiByte(
        CP_UTF8,
        0,
        widePath,
        -1,
        path,
        MAX_PATH,
        nullptr,
        nullptr);

    CoTaskMemFree(widePath);

    psi->Release();
    pfd->Release();

    return std::string(path);
}

bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}
