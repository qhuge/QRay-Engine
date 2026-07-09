#include "world.hpp"
#include <fstream>
#include <sstream>
#include <string>

std::vector<Tile> worldWalls;

//grid used to speedup locating tiles on specific coordinates
int worldGrid[64][64];

std::vector<Entity> worldEntities;

bool TileMatches(const Tile& tile, int x, int y)
{
    return tile.x == x && tile.y == y;
}

//btw note to the future employer: this code below right here tripled the fps!!!!!!!!! 
int FindWall(int x, int y)
{
    return worldGrid[x][y];

    //old implementation (slow) (idk who came up with this crap...)
    /*for (int i = 0; i < worldWalls.size(); i++)
    {
        if (worldWalls[i].x == x &&
            worldWalls[i].y == y)
        {
            return i;
        }
    }
    return -1;*/
}

bool LoadWorld(const char* filename)
{
    worldWalls.clear();
    worldEntities.clear();

    std::ifstream file(filename);

    if (!file.is_open())
        return false;

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::stringstream ss(line);

        char type;
        ss >> type;

        switch (type)
        {
        case 'T':
        {
            Tile tile;

            ss >> tile.x >> tile.y >> tile.textureIndex;

            worldWalls.push_back(tile);
            break;
        }

        case 'E':
        {
            Entity entity;

            ss >> entity.x >> entity.y >> entity.textureIndex;

            entity.x += 0.5f;
            entity.y += 0.5f;

            //set the target location to be the same as the current position so the movement code will take over!
            entity.targetX = entity.x;
            entity.targetY = entity.y;

            entity.entityTypeIndex = entity.textureIndex - (cfg.textureAmount - cfg.entityAmount);

            entity.health = gEntityTypes[entity.entityTypeIndex].health;

            //set the animation max index
            entity.maxFrameIndex = gTextures[entity.textureIndex].frames - 1;

            if (entity.maxFrameIndex > 0) {
                entity.animate = true;
                entity.maxAnimationDuration = gTextures[entity.textureIndex].maxAnimationDuration;
            }

            worldEntities.push_back(entity);
            break;
        }
        case 'D':
        {
            Tile tile;
            ss >> tile.x >> tile.y >> tile.textureIndex;

            int doorDir;
            int doorFace;

            ss >> doorDir >> doorFace;

            tile.isDoor = true;

            //THIS MEANS DIRECTION OF THE INDIVIUAL DOOR NOT THE 2 DOOR TILES TOGETHER (IF BOTH TILES HAVE SAME Y THEN DIR = HORIZONTAL)
            switch (doorDir)
            {
                case 0: tile.door.dir = Axis::Horizontal; break;
                case 1: tile.door.dir = Axis::Vertical; break;
                default: return false;
            }

            switch (doorFace)
            {
                case 0: tile.door.renderedFace = Direction::Up; break;
                case 1: tile.door.renderedFace = Direction::Right; break;
                case 2: tile.door.renderedFace = Direction::Down; break;
                case 3: tile.door.renderedFace = Direction::Left; break;
                default: return false;
            }
            tile.door.open = 0.0f;

            //set the tag
            ss >> tile.door.tag;

            worldWalls.push_back(tile);
            break;
        }

        //TODO spawn based on map file:
        /*case 'S':
        {
            ss >> spawnX >> spawnY;
            break;
        }*/

        default:
            return false;
        }
    }

    //initialize the grid
    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            worldGrid[x][y] = -1;
        }
    }
    //then set the proper indecies
    for (int i = 0; i < worldWalls.size(); i++)
    {
        worldGrid[worldWalls[i].x][worldWalls[i].y] = i;
    }

    //set door indexes
    for (int i = 0; i < worldWalls.size(); i++) {
        Tile& tile = worldWalls[i];

        if (!tile.isDoor) {
            continue;
        } 

        if (tile.door.dir == Axis::Horizontal) {
            if (tile.door.renderedFace == Direction::Right) {
                int indexOfNeighbor = FindWall(tile.x + 1, tile.y);
                if (indexOfNeighbor != -1) {
                    tile.door.indexOfOtherDoorTile = indexOfNeighbor;
                }
                else {
                    return false;
                }
            } else if (tile.door.renderedFace == Direction::Left) {
                int indexOfNeighbor = FindWall(tile.x - 1, tile.y);
                if (indexOfNeighbor != -1) {
                    tile.door.indexOfOtherDoorTile = indexOfNeighbor;
                }
                else {
                    return false;
                }
            } else { return false; }
        } else if (tile.door.dir == Axis::Vertical) {
            if (tile.door.renderedFace == Direction::Up) {
                int indexOfNeighbor = FindWall(tile.x, tile.y - 1);
                if (indexOfNeighbor != -1) {
                    tile.door.indexOfOtherDoorTile = indexOfNeighbor;
                }
                else {
                    return false;
                }
            }
            else if (tile.door.renderedFace == Direction::Down) {
                int indexOfNeighbor = FindWall(tile.x, tile.y + 1);
                if (indexOfNeighbor != -1) {
                    tile.door.indexOfOtherDoorTile = indexOfNeighbor;
                }
                else {
                    return false;
                }
            }
            else { return false; }
        }
    }

    return true;
}

Texture LoadQRayAsset(std::string path)
{
    std::ifstream file(path, std::ios::binary);

    QRayTextureAssetHeader header;
    file.read((char*)&header, sizeof(header));

    Texture tex;
    tex.width = header.width;
    tex.height = header.height;
    tex.frames = header.frames;
    tex.maxAnimationDuration = header.maxAnimationDuration;

    //size of 1 frame
    size_t pixelCount = header.width * header.height;

    tex.pixels.resize(header.frames);

    //read all the frames
    for (int i = 0; i < header.frames; i++) {
        tex.pixels[i].resize(pixelCount);
        file.read(reinterpret_cast<char*>(tex.pixels[i].data()), pixelCount * sizeof(uint32_t));
    }
    

    return tex;
}

EntityType LoadQRayEntity(std::string path)
{
    std::ifstream file(path, std::ios::binary);

    EntityType entityT;

    file.read((char*)&entityT, sizeof(entityT));

    file.close();

    return entityT;
}