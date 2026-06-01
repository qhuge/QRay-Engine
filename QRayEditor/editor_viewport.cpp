#include "editor_viewport.hpp"
#include "imgui.h"
#include "editor_helpers.hpp"

#include <vector>
#include "world.hpp"
#include "config.hpp"
#include "editor_main.hpp"

// simple grid settings
static float gridSize = 32.0f;
const float TILE_SIZE = 32.0f;
const float RADIUS = 8.0f;
static ImVec2 cameraOffset(0.0f, 0.0f);
const int MAP_W = 100;
const int MAP_H = 100;

void DrawGrid(ImDrawList* drawList, ImVec2 origin, ImVec2 size)
{
    ImU32 color = IM_COL32(60, 60, 60, 255);

    float offsetX = fmodf(-cameraOffset.x, gridSize);
    float offsetY = fmodf(-cameraOffset.y, gridSize);

    if (offsetX < 0) offsetX += gridSize;
    if (offsetY < 0) offsetY += gridSize;

    // IMPORTANT: subtract offset to align with world movement
    float startX = origin.x - offsetX;
    float startY = origin.y - offsetY;

    for (float x = startX; x < origin.x + size.x; x += gridSize)
    {
        drawList->AddLine(
            ImVec2(x, origin.y),
            ImVec2(x, origin.y + size.y),
            color
        );
    }

    for (float y = startY; y < origin.y + size.y; y += gridSize)
    {
        drawList->AddLine(
            ImVec2(origin.x, y),
            ImVec2(origin.x + size.x, y),
            color
        );
    }
}

void DrawViewport()
{
    static bool initialized = false;

    if (!initialized)
    {
        ImVec2 screen = ImGui::GetIO().DisplaySize;

        cameraOffset.x = (screen.x - MAP_W * TILE_SIZE) * 0.5f;
        cameraOffset.y = (screen.y - MAP_H * TILE_SIZE) * 0.5f;

        initialized = true;
    }

    ImVec2 origin = ImGui::GetCursorScreenPos();

    ImVec2 size = ImGui::GetContentRegionAvail();
    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImDrawList* draw = ImGui::GetWindowDrawList();

    // background
    draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
        IM_COL32(25, 25, 25, 255));

    // grid
    DrawGrid(draw, pos, size);

    for (const Tile& tile : save.tiles)
    {
        ImVec2 topLeft(
            pos.x + cameraOffset.x + tile.x * TILE_SIZE,
            pos.y + cameraOffset.y + tile.y * TILE_SIZE
        );

        ImVec2 bottomRight(
            topLeft.x + TILE_SIZE,
            topLeft.y + TILE_SIZE
        );

        ImU32 color = IM_COL32(255, 255, 0, 255);

        if (tile.tileTypeIndex != -1) {
            TileType& tileType = save.tileTypes[tile.tileTypeIndex];
            color = IM_COL32(tileType.r, tileType.g, tileType.b, 255);;
        }

        draw->AddRectFilled(topLeft, bottomRight, color);
        draw->AddRect(topLeft, bottomRight, IM_COL32(0, 0, 0, 255));
    }

    for (const Entity& entity : save.entities)
    {
        ImVec2 center(
            pos.x + cameraOffset.x + entity.x * TILE_SIZE + TILE_SIZE * 0.5f,
            pos.y + cameraOffset.y + entity.y * TILE_SIZE + TILE_SIZE * 0.5f
        );

        ImU32 color;

        switch (entity.entityTypeIndex)
        {
        case 0: color = IM_COL32(255, 220, 50, 255); break;
        case 1: color = IM_COL32(50, 255, 120, 255); break;
        case 2: color = IM_COL32(255, 80, 80, 255); break;
        default: color = IM_COL32(220, 220, 220, 255); break;
        }

        draw->AddCircleFilled(center, RADIUS, color, 12);
        draw->AddCircle(center, RADIUS, IM_COL32(0, 0, 0, 255), 12, 1.5f);
    }

    if (ImGui::IsWindowHovered())
    {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            cameraOffset.x += delta.x;
            cameraOffset.y += delta.y;
        }
    }


    //hovering and clicking
    ImVec2 mouse = ImGui::GetMousePos();

    int tileX = (int)((mouse.x - pos.x - cameraOffset.x) / TILE_SIZE);
    int tileY = (int)((mouse.y - pos.y - cameraOffset.y) / TILE_SIZE);

    bool inside = ImGui::IsWindowHovered() && tileX >= 0 && tileY >= 0 && tileX < MAP_W && tileY < MAP_H;

    if (inside)
    {
        ImVec2 topLeft(
            pos.x + cameraOffset.x + tileX * TILE_SIZE,
            pos.y + cameraOffset.y + tileY * TILE_SIZE
        );

        draw->AddRect(
            topLeft,
            ImVec2(topLeft.x + TILE_SIZE, topLeft.y + TILE_SIZE),
            IM_COL32(255, 255, 0, 255),
            0.0f,
            0,
            2.0f
        );
        
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (FindWall(tileX, tileY) == -1 && FindEntity(tileX, tileY) == -1)
            {
                if (gCreateMode == EditorCreateMode::Tile) {
                    Tile t;
                    t.x = tileX;
                    t.y = tileY;
                    t.tileTypeIndex = gSelectedTileType;

                    save.tiles.push_back(t);

                    save.tileTypes[t.tileTypeIndex].timesUsed++;

                } else if (gCreateMode == EditorCreateMode::Spawnpoint && !save.hasPlacedSpawnpoint) {
                    Tile t;
                    t.x = tileX;
                    t.y = tileY;
                    t.tileTypeIndex = -1;

                    save.tiles.push_back(t);

                    save.hasPlacedSpawnpoint = true;
                } else if (gCreateMode == EditorCreateMode::Entity) {
                    Entity e;
                    e.x = tileX;
                    e.y = tileY;
                    e.entityTypeIndex = gSelectedEntityType;

                    save.entities.push_back(e);

                    save.entityTypes[e.entityTypeIndex].timesUsed++;
                }
            }
        } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            int tileIndex = FindWall(tileX, tileY);
            if (tileIndex != -1) {
                if (save.tiles[tileIndex].tileTypeIndex == -1) {
                    save.hasPlacedSpawnpoint = false;
                }
                else {
                    save.tileTypes[save.tiles[tileIndex].tileTypeIndex].timesUsed--;
                }

                save.tiles.erase(save.tiles.begin() + tileIndex);
            }
            else {
                int entityIndex = FindEntity(tileX, tileY);
                if (entityIndex != -1) {
                    save.entityTypes[save.entities[entityIndex].entityTypeIndex].timesUsed--;

                    save.entities.erase(save.entities.begin() + entityIndex);
                }
            }
        }
    }
    

    ImGui::InvisibleButton("viewport_click_area", size);
}