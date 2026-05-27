#include "editor_renderer.h"
#include "editor_map.h"
#include "config.h"
#include "editor_viewport.h"
int abcd = 123;
void RenderEditor(HDC hdc, HWND hWnd)
{
    RECT clientRect;

    GetClientRect(hWnd, &clientRect);

    HBRUSH bgBrush = CreateSolidBrush(RGB(30, 30, 30));

    FillRect(hdc, &clientRect, bgBrush);

    DeleteObject(bgBrush);

    //calc
    int viewportWidth = clientRect.right;
    int viewportHeight = clientRect.bottom;

    int startX = gCameraX / TILE_SIZE;
    int startY = gCameraY / TILE_SIZE;

    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;

    int endX = startX + (viewportWidth / TILE_SIZE) + 2;

    int endY = startY + (viewportHeight / TILE_SIZE) + 2;

    if (endX > MAP_WIDTH)
        endX = MAP_WIDTH;

    if (endY > MAP_HEIGHT)
        endY = MAP_HEIGHT;

    //render tiles
    for (int y = startY; y < endY; y++)
    {
        for (int x = startX; x < endX; x++)
        {
            int screenX =
                (x * TILE_SIZE) - gCameraX;

            int screenY =
                (y * TILE_SIZE) - gCameraY;

            RECT rect =
            {
                screenX,
                screenY,
                screenX + TILE_SIZE,
                screenY + TILE_SIZE
            };

            // draw wall tiles
            int wallIndex = FindWall(x, y);
            if (wallIndex != -1)
            {
                HBRUSH brush;
                 
                if (worldMap[wallIndex].textureIndex == -1) {
                    brush = CreateSolidBrush(RGB(255, 255, 0));
                }
                else {
                    brush = CreateSolidBrush(RGB(220, 220, 220));
                }

                FillRect(hdc, &rect, brush);

                DeleteObject(brush);
            }

            // draw grid lines
            FrameRect(
                hdc,
                &rect,
                (HBRUSH)GetStockObject(BLACK_BRUSH));
        }
    }
}