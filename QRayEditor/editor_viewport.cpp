#include "editor_viewport.hpp"
#include "imgui.h"
#include "editor_helpers.hpp"

#include <vector>
#include "world.hpp"
#include "config.hpp"
#include "editor_main.hpp"
#include "editor_popups_error.hpp"
#include <algorithm>

// simple grid settings
static float gridSize = 32.0f;
const float TILE_SIZE = 32.0f;
const float RADIUS = 8.0f;
static ImVec2 cameraOffset(0.0f, 0.0f);
const int MAP_W = 64;
const int MAP_H = 64;

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

        // Empty / special tile
        if (tile.tileTypeIndex == -1)
        {
            draw->AddRectFilled(
                topLeft,
                bottomRight,
                IM_COL32(255, 255, 0, 255) // Yellow
            );

            draw->AddRect(
                topLeft,
                bottomRight,
                IM_COL32(0, 0, 0, 255)
            );

            continue;
        }

        //ignore the rest of negative tiletypes
        if (tile.tileTypeIndex < 0) {
            continue;
        }

        TileType& tileType = save.tileTypes[tile.tileTypeIndex];

        // Door rendering
        if (tileType.isDoor)
        {
            const float thickness = 4.0f;
            ImU32 doorColor = IM_COL32(tileType.r, tileType.g, tileType.b, 255);

            if (tile.doorDirection == DoorDirection::Horizontal) {
                draw->AddLine(
                    ImVec2(bottomRight.x, topLeft.y),
                    ImVec2(bottomRight.x, bottomRight.y),
                    doorColor,
                    thickness
                );
            }
            else if (tile.doorDirection == DoorDirection::Vertical) {
                draw->AddLine(
                    ImVec2(bottomRight.x - TILE_SIZE, topLeft.y + TILE_SIZE),
                    ImVec2(bottomRight.x, bottomRight.y),
                    doorColor,
                    thickness
                );
            }


            continue;
        }

        //get the tiletype color
        ImU32 color = IM_COL32(tileType.r, tileType.g, tileType.b, 255);

        //render the rectangle
        draw->AddRectFilled(topLeft, bottomRight, color);
        draw->AddRect(topLeft, bottomRight, IM_COL32(0, 0, 0, 255));
    }

    for (const Entity& entity : save.entities)
    {
        ImVec2 center(
            pos.x + cameraOffset.x + entity.x * TILE_SIZE + TILE_SIZE * 0.5f,
            pos.y + cameraOffset.y + entity.y * TILE_SIZE + TILE_SIZE * 0.5f
        );

        //get the color
        ImU32 color = IM_COL32(save.entityTypes[entity.entityTypeIndex].r, save.entityTypes[entity.entityTypeIndex].g, save.entityTypes[entity.entityTypeIndex].b, 255);

        //render the entity
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
        if (ImGui::IsWindowHovered() && ImGui::IsKeyPressed(ImGuiKey_R))
        {
            if (gDoorDirection == 0) {
                gDoorDirection = 1;
            }
            else if (gDoorDirection == 1) {
                gDoorDirection = 0;
            }
        }

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

        //check if door is selected and in that case draw another hover rect
        if (gSelectedTileType != -1) {
            if (gCreateMode == EditorCreateMode::Tile && save.tileTypes[gSelectedTileType].isDoor) {
                if (gDoorDirection == 0) {
                    draw->AddRect(
                        topLeft,
                        ImVec2(topLeft.x + TILE_SIZE * 2, topLeft.y + TILE_SIZE),
                        IM_COL32(255, 255, 0, 255),
                        0.0f,
                        0,
                        2.0f
                    );
                }
                else if (gDoorDirection == 1) {
                    draw->AddRect(
                        topLeft,
                        ImVec2(topLeft.x + TILE_SIZE, topLeft.y + TILE_SIZE * 2),
                        IM_COL32(255, 255, 0, 255),
                        0.0f,
                        0,
                        2.0f
                    );
                }
                
            }
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (FindWall(tileX, tileY) == -1 && FindEntity(tileX, tileY) == -1)
            {    
                //gSelectedTileType is initialized as -1. This needs to be checked or else the program crashes when clicking at start!!
                if (gCreateMode == EditorCreateMode::Tile && gSelectedTileType != -1) {
                    bool& isDoor = save.tileTypes[gSelectedTileType].isDoor;
                    bool validTile = true;

                    Tile t;

                    //ensure there's enough space for the door :D
                    if (isDoor) {
                        if (gDoorDirection == 0) {

                            if (FindWall(tileX + 1, tileY) != -1 || FindEntity(tileX + 1, tileY) != -1) {
                                validTile = false;
                            }

                            t.doorDirection = DoorDirection::Horizontal;

                            //create a fake tile for the other part of the door, this is needed for the findwall checks to work
                            Tile t2;
                            t2.x = tileX + 1;
                            t2.y = tileY;
                            t2.tileTypeIndex = -2;// negative types are not rendered and other than -1 are also not built
                            save.tiles.push_back(t2);

                        }
                        else {

                            if (FindWall(tileX, tileY + 1) != -1 || FindEntity(tileX, tileY + 1) != -1) {
                                validTile = false;
                            }

                            t.doorDirection = DoorDirection::Vertical;

                            //create a fake tile for the other part of the door, this is needed for the findwall checks to work
                            Tile t2;
                            t2.x = tileX;
                            t2.y = tileY + 1;
                            t2.tileTypeIndex = -2;// negative types are not rendered and other than -1 are also not built
                            save.tiles.push_back(t2);
                        }
                    }

                    if (validTile) {
                        
                        t.x = tileX;
                        t.y = tileY;
                        t.tileTypeIndex = gSelectedTileType;

                        save.tiles.push_back(t);

                        save.tileTypes[t.tileTypeIndex].timesUsed++;
                    }
                    

                } else if (gCreateMode == EditorCreateMode::Spawnpoint) {
                    if (!save.hasPlacedSpawnpoint) {
                        Tile t;
                        t.x = tileX;
                        t.y = tileY;
                        t.tileTypeIndex = -1;

                        save.tiles.push_back(t);

                        save.hasPlacedSpawnpoint = true;
                    }
                    else {
                        ShowError("Cant place tile", "You can only place 1 spawnpoint tile");
                    }
                    
                } else if (gCreateMode == EditorCreateMode::Entity && gSelectedEntityType != -1) {
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
            //ignore -2 as that cant directly be deleted. TODO: make a list of undeletable tiletypes and check against that
            if (tileIndex != -1 && save.tiles[tileIndex].tileTypeIndex != -2) {

                std::vector<int> indicesToDelete;

                if (save.tiles[tileIndex].tileTypeIndex == -1) {
                    save.hasPlacedSpawnpoint = false;
                }
                else {
                    save.tileTypes[save.tiles[tileIndex].tileTypeIndex].timesUsed--;
                
                    if (save.tileTypes[save.tiles[tileIndex].tileTypeIndex].isDoor) {
                        if (save.tiles[tileIndex].doorDirection == DoorDirection::Horizontal) {
                            int secondDoorTileIndex = FindWall(tileX + 1, tileY);
                            indicesToDelete.push_back(secondDoorTileIndex);
                        }
                        else {
                            int secondDoorTileIndex = FindWall(tileX, tileY + 1);
                            indicesToDelete.push_back(secondDoorTileIndex);
                        }
                    }
                }
                
                indicesToDelete.push_back(tileIndex);

                std::sort(indicesToDelete.rbegin(), indicesToDelete.rend());

                for (int idx : indicesToDelete)
                {
                    save.tiles.erase(save.tiles.begin() + idx);
                }       
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