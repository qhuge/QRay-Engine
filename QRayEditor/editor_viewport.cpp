#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

#include "editor_viewport.h"
#include "editor_renderer.h"
#include "editor_map.h"
#include "config.h"

int gCameraX = 0;
int gCameraY = 0;

bool gDragging = false;

int gLastMouseX = 0;
int gLastMouseY = 0;

int gSelectedBlockType = 0;

bool hasSelectedSpawnPoint = false;

LRESULT CALLBACK ViewportWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
        return 1;
    case WM_LBUTTONDOWN:
    {
        int mouseX = LOWORD(lParam);
        int mouseY = HIWORD(lParam);

        int worldX = mouseX + gCameraX;
        int worldY = mouseY + gCameraY;

        int tileX = worldX / TILE_SIZE;
        int tileY = worldY / TILE_SIZE;

        if (tileX >= 0 &&
            tileX < MAP_WIDTH &&
            tileY >= 0 &&
            tileY < MAP_HEIGHT &&
            FindWall(tileX, tileY) == -1)
        {
            //spawnpoint
            if (gSelectedBlockType == 3) {
                if (!hasSelectedSpawnPoint) {
                    cfg.playerX = tileX;
                    cfg.playerY = tileY;
                    hasSelectedSpawnPoint = true;
                    worldMap.push_back(
                        {
                            tileX,
                            tileY,
                            -1//-1 ignored in build
                        });
                }
                
            }
            else {
                worldMap.push_back(
                    {
                        tileX,
                        tileY,
                        0
                    });
            }
            
        }

        InvalidateRect(hWnd, nullptr, FALSE);
    }
    return 0;

    case WM_RBUTTONDOWN:
    {
        int mouseX = LOWORD(lParam);
        int mouseY = HIWORD(lParam);

        int worldX = mouseX + gCameraX;
        int worldY = mouseY + gCameraY;

        int tileX = worldX / TILE_SIZE;
        int tileY = worldY / TILE_SIZE;

        int wallIndex =
            FindWall(tileX, tileY);

        if (wallIndex != -1)
        {
            if (worldMap[wallIndex].textureIndex == -1) {
                hasSelectedSpawnPoint = false;
            }
            worldMap.erase(worldMap.begin() + wallIndex);
        }

        InvalidateRect(hWnd, nullptr, FALSE);
    }
    return 0;

    case WM_MBUTTONDOWN:
    {
        gDragging = true;

        gLastMouseX = LOWORD(lParam);
        gLastMouseY = HIWORD(lParam);

        SetCapture(hWnd);

        return 0;
    }
    case WM_MBUTTONUP:
    {
        gDragging = false;

        ReleaseCapture();

        return 0;
    }
    case WM_MOUSEMOVE:
    {
        if (gDragging)
        {
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);

            int deltaX = mouseX - gLastMouseX;
            int deltaY = mouseY - gLastMouseY;

            gCameraX -= deltaX;
            gCameraY -= deltaY;

            gLastMouseX = mouseX;
            gLastMouseY = mouseY;

            InvalidateRect(hWnd, nullptr, FALSE);
        }

        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;

        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rect;
        GetClientRect(hWnd, &rect);

        int width = rect.right;
        int height = rect.bottom;

        // create back buffer
        HDC memDC = CreateCompatibleDC(hdc);

        HBITMAP memBitmap =
            CreateCompatibleBitmap(
                hdc,
                width,
                height);

        HBITMAP oldBitmap =
            (HBITMAP)SelectObject(
                memDC,
                memBitmap);

        // render into memory DC
        RenderEditor(memDC, hWnd);

        // copy final image to screen
        BitBlt(
            hdc,
            0,
            0,
            width,
            height,
            memDC,
            0,
            0,
            SRCCOPY);

        // cleanup
        SelectObject(memDC, oldBitmap);

        DeleteObject(memBitmap);

        DeleteDC(memDC);

        EndPaint(hWnd, &ps);

        return 0;
    }
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}