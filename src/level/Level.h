#pragma once

#include <iostream>
#include <vector>

#include "Mesh.h"
#include "Room.h"
#include "BinarySpacePartition.h"


class Level
{
private:
    int width;
    int height;

    std::vector<Room> rooms;
    BinarySpacePartition bsp;
    

public:
    Level(int width, int height)
        : width(width),
          height(height),
          rooms(),
          bsp(width, height, rooms)
    {
    }

    BinarySpacePartition& getBsp()
    {
        return bsp;
    }

    void printRooms() const
    {
        for (const auto& room : rooms)
        {
            std::cout << "Room ID: " << room.getId()
                      << ", Position: (" << room.getX()
                      << ", " << room.getY() << ")"
                      << ", Size: (" << room.getWidth()
                      << "x" << room.getHeight() << ")"
                      << std::endl;
        }
    }

template <typename Predicate>
Mesh generateFloorMeshFiltered(Predicate include, float textureScale = 4.0f) const
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    layout.Push<float>(3); // normals
    layout.Push<float>(2); // texture coordinates

    const auto& space = bsp.getSpace();
    int gridHeight = static_cast<int>(space.size());
    if (gridHeight == 0) return Mesh(vertices, indices);
    int gridWidth = static_cast<int>(space[0].size());

    for (int y = 0; y < gridHeight; ++y)
    {
        for (int x = 0; x < gridWidth; ++x)
        {
            if (!include(space[y][x])) continue;

            unsigned int vertexOffset = static_cast<unsigned int>(vertices.size() / 8);
            float fx = static_cast<float>(x);
            float fy = static_cast<float>(y);

            float u0 = fx / textureScale;
            float u1 = (fx + 1.0f) / textureScale;
            float v0 = fy / textureScale;
            float v1 = (fy + 1.0f) / textureScale;

            vertices.insert(vertices.end(), { // pos+normal+texcoord
                fx,        0.0f, fy,        0.0f, 1.0f,0.0f, u0, v0,
                fx + 1.0f, 0.0f, fy,        0.0f, 1.0f,0.0f, u1, v0,
                fx + 1.0f, 0.0f, fy + 1.0f, 0.0f, 1.0f,0.0f, u1, v1,
                fx,        0.0f, fy + 1.0f, 0.0f, 1.0f,0.0f, u0, v1
            });

            indices.insert(indices.end(), {
                vertexOffset + 0, vertexOffset + 1, vertexOffset + 2,
                vertexOffset + 2, vertexOffset + 3, vertexOffset + 0
            });
        }
    }

    return Mesh(vertices, indices, layout);
}
    Mesh generateFloorMesh() const
    {
        return generateFloorMeshFiltered([](int cell) { return cell > 0; });
    }

    Mesh generateCorridorMesh() const
    {
        return generateFloorMeshFiltered([](int cell) { return cell == -1; });
    }

    Mesh generateWallMesh(float wallHeight = 3.0f, float wallTextureScale = 2.0f) const
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        VertexBufferLayout layout;
        layout.Push<float>(3); // position
        layout.Push<float>(2); // texture coordinates
        const auto& space = bsp.getSpace();
        int gridHeight = static_cast<int>(space.size());
        if (gridHeight == 0) return Mesh(vertices, indices);
        int gridWidth = static_cast<int>(space[0].size());

        auto isFloor = [&](int x, int y) -> bool {
            if (x < 0 || y < 0 || x >= gridWidth || y >= gridHeight) return false;
            return space[y][x] != 0;
        };
            auto addWallQuad = [&](float x0, float z0, float x1, float z1)
            {
                unsigned int vertexOffset = static_cast<unsigned int>(vertices.size() / 5);

                float wallLength = std::sqrt((x1 - x0) * (x1 - x0) + (z1 - z0) * (z1 - z0));
                float u1 = wallLength / wallTextureScale;
                float v1 = wallHeight / wallTextureScale;

                vertices.insert(vertices.end(), {
                    x0, 0.0f,       z0, 0.0f, 0.0f,
                    x1, 0.0f,       z1, u1,   0.0f,
                    x1, wallHeight, z1, u1,   v1,
                    x0, wallHeight, z0, 0.0f, v1
                });
                indices.insert(indices.end(), {
                    vertexOffset + 0, vertexOffset + 2, vertexOffset + 1,
                    vertexOffset + 2, vertexOffset + 0, vertexOffset + 3
                });
            };

        for (int y = 0; y < gridHeight; ++y)
        {
            for (int x = 0; x < gridWidth; ++x)
            {
                if (!isFloor(x, y)) continue;

                if (!isFloor(x, y - 1)) // north
                    addWallQuad((float)x, (float)y, (float)(x + 1), (float)y);
                if (!isFloor(x, y + 1)) // south
                    addWallQuad((float)(x + 1), (float)(y + 1), (float)x, (float)(y + 1));
                if (!isFloor(x - 1, y)) // west
                    addWallQuad((float)x, (float)(y + 1), (float)x, (float)y);
                if (!isFloor(x + 1, y)) // east
                    addWallQuad((float)(x + 1), (float)y, (float)(x + 1), (float)(y + 1));
            }
        }

        return Mesh(vertices, indices, layout);
    }

};