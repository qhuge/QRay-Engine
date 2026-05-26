#include "editor_renderer.h"
#include "editor_map.h"
#include "config.h"
int a = 1243;
void RenderEditor(HDC hdc)
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            RECT rect =
            {
                x * TILE_SIZE,
                y * TILE_SIZE,
                (x + 1) * TILE_SIZE,
                (y + 1) * TILE_SIZE
            };

            HBRUSH brush;

            if (FindWall(x, y) != -1)
            {
                brush = CreateSolidBrush(RGB(255, 255, 255));
            }
            else
            {
                brush = CreateSolidBrush(RGB(40, 40, 40));
            }

            FillRect(hdc, &rect, brush);

            DeleteObject(brush);

            FrameRect(hdc, &rect,
                (HBRUSH)GetStockObject(BLACK_BRUSH));
        }
    }
}