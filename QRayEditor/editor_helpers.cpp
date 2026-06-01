#include <windows.h>
#include <shobjidl.h>

#include "editor_helpers.hpp"
#include "world.hpp"
#include "config.hpp"
#include <string>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;
#include <imgui.h>
#include "editor_main.hpp"

int FindWall(int x, int y)
{
    for (int i = 0; i < save.tiles.size(); i++)
    {
        if (save.tiles[i].x == x &&
            save.tiles[i].y == y)
        {
            return i;
        }
    }
    return -1;
}

int FindEntity(int x, int y)
{
    for (int i = 0; i < save.entities.size(); i++)
    {
        if (save.entities[i].x == x &&
            save.entities[i].y == y)
        {
            return i;
        }
    }
    return -1;
}

void SaveProject(const std::string& path)
{
    std::string savePath = path + "/project.qray";

    std::ofstream out(savePath, std::ios::binary);

    auto writeVec = [&](auto& vec)
        {
            size_t size = vec.size();
            out.write((char*)&size, sizeof(size));
            out.write((char*)vec.data(), size * sizeof(vec[0]));
        };

    out.write((char*)&save.gameTitle, sizeof(save.gameTitle));
    out.write((char*)&save.projectTitle, sizeof(save.projectTitle));
    out.write((char*)&save.resolutionX, sizeof(save.resolutionX));
    out.write((char*)&save.resolutionY, sizeof(save.resolutionY));
    out.write((char*)&save.startingAngle, sizeof(save.startingAngle));
    out.write((char*)&save.renderDistance, sizeof(save.renderDistance));
    out.write((char*)&save.hasPlacedSpawnpoint, sizeof(save.hasPlacedSpawnpoint));

    writeVec(save.tiles);
    writeVec(save.entities);
    writeVec(save.tileTypes);
    writeVec(save.entityTypes);
}


static std::string iniPath;

void LoadProject(const std::string& path, HWND g_hWnd)
{
    ProjectFile p;

    std::ifstream in(path, std::ios::binary);

    auto readVec = [&](auto& vec)
        {
            size_t size;
            in.read((char*)&size, sizeof(size));
            vec.resize(size);
            in.read((char*)vec.data(), size * sizeof(vec[0]));
        };

    in.read((char*)&p.gameTitle, sizeof(p.gameTitle));
    in.read((char*)&p.projectTitle, sizeof(p.projectTitle));
    in.read((char*)&p.resolutionX, sizeof(int));
    in.read((char*)&p.resolutionY, sizeof(int));
    in.read((char*)&p.startingAngle, sizeof(int));
    in.read((char*)&p.renderDistance, sizeof(int));
    in.read((char*)&p.hasPlacedSpawnpoint, sizeof(bool));

    readVec(p.tiles);
    readVec(p.entities);
    readVec(p.tileTypes);
    readVec(p.entityTypes);

    save = p;

    //set window title and imgui ini file location
    std::string title = std::string("QRay Editor (") + save.projectTitle + ")";
    std::wstring wtitle(title.begin(), title.end());
    SetWindowTextW(g_hWnd, wtitle.c_str());

    iniPath = saveLocation + "/editor_layout.ini";

    ImGui::GetIO().IniFilename = iniPath.c_str();
}

std::string SelectFolder()
{
    std::string result;

    IFileDialog* pfd = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_FileOpenDialog,
        nullptr,
        CLSCTX_ALL,
        IID_PPV_ARGS(&pfd));

    if (SUCCEEDED(hr))
    {
        DWORD options;
        pfd->GetOptions(&options);
        pfd->SetOptions(options | FOS_PICKFOLDERS);

        hr = pfd->Show(nullptr);

        if (SUCCEEDED(hr))
        {
            IShellItem* item = nullptr;

            if (SUCCEEDED(pfd->GetResult(&item)))
            {
                PWSTR path = nullptr;

                if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)))
                {
                    std::wstring wpath(path);

                    result.assign(wpath.begin(), wpath.end());

                    CoTaskMemFree(path);
                }

                item->Release();
            }
        }

        pfd->Release();
    }

    return result;
}

std::string SelectProjectFile()
{
    OPENFILENAMEW ofn = {};
    wchar_t fileName[MAX_PATH] = L"";

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr; // or your editor HWND
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;

    ofn.lpstrFilter =
        L"QRay Project (*.qray)\0*.qray\0"
        L"All Files (*.*)\0*.*\0";

    ofn.Flags =
        OFN_FILEMUSTEXIST |
        OFN_PATHMUSTEXIST |
        OFN_EXPLORER;

    if (GetOpenFileNameW(&ofn))
    {
        std::wstring wpath(fileName);
        return std::string(wpath.begin(), wpath.end());
    }

    return "";
}

void CreateNewProject(const std::string& folder, std::string projectName)
{
    //replace spaces for filenames
    std::replace(projectName.begin(), projectName.end(), ' ', '_');

    fs::create_directories(folder + "\\" + projectName + "/assets");

    saveLocation = folder + "\\" + projectName;

    strcpy_s(save.projectTitle, projectName.c_str());

    SaveProject(saveLocation);
}

std::string OpenFileDialogPNG()
{
    char filename[MAX_PATH] = "";

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;

    ofn.lpstrFilter = "PNG Files\0*.png\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;

    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn))
    {
        return std::string(filename);
    }

    return "";
}

bool CopyAssetToProject(const std::string& sourcePath, const std::string& projectFolder, std::string& outRelativePath)
{
    try
    {
        fs::path src(sourcePath);

        if (!fs::exists(src))
            return false;

        // ensure assets folder exists
        fs::path assetsDir = fs::path(projectFolder) / "assets";
        fs::create_directories(assetsDir);

        // avoid name conflicts (optional but recommended)
        std::string filename = GenerateAssetName(src.filename().string());
        fs::path dest = assetsDir / filename;

        fs::copy_file(src, dest, fs::copy_options::overwrite_existing);

        outRelativePath = "assets/" + filename;

        return true;
    }
    catch (...)
    {
        return false;
    }
}

std::string GenerateAssetName(const std::string& original)
{
    int r = rand();

    return std::to_string(r) + "_" + original;
}

void SetRandomColorForTile(TileType& t) {
    t.r = (rand() % 216) + 40;
    t.g = (rand() % 216) + 40;
    t.b = (rand() % 216) + 40;
}

void SetRandomColorForEntity(EntityType& e) {
    e.r = (rand() % 216) + 40;
    e.g = (rand() % 216) + 40;
    e.b = (rand() % 216) + 40;
}

void SetEditorMode(EditorCreateMode mode)
{
    gCreateMode = mode;

    if (mode != EditorCreateMode::Tile)
        gSelectedTileType = -1;

    if (mode != EditorCreateMode::Entity)
        gSelectedEntityType = -1;
}

bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

bool FloatEquals(float a, float b)
{
    return std::abs(a - b) < 0.001f;
}
