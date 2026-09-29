#pragma once

#include <vector>
#include "../Mesh.h"
#include "../BinarySpacePartition.h"
#include <cmath>

class LevelMeshGenerator
{
public:
    template <typename Predicate>
    static Mesh generateFloorMeshFiltered(const BinarySpacePartition& bsp, Predicate include, float textureScale = 4.0f)
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

    static Mesh generateFloorMesh(const BinarySpacePartition& bsp);
    static Mesh generateCorridorMesh(const BinarySpacePartition& bsp);
    static Mesh generateWallMesh(const BinarySpacePartition& bsp, float wallHeight = 5.0f, float wallTextureScale = 2.0f);
};
