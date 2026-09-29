#include "LevelMeshGenerator.h"

Mesh LevelMeshGenerator::generateFloorMesh(const BinarySpacePartition& bsp)
{
    return generateFloorMeshFiltered(bsp, [](int cell) { return cell > 0; });
}

Mesh LevelMeshGenerator::generateCorridorMesh(const BinarySpacePartition& bsp)
{
    return generateFloorMeshFiltered(bsp, [](int cell) { return cell == -1; });
}

Mesh LevelMeshGenerator::generateWallMesh(const BinarySpacePartition& bsp, float wallHeight, float wallTextureScale)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    layout.Push<float>(3); // normal
    layout.Push<float>(2); // texture coordinates
    const auto& space = bsp.getSpace();
    int gridHeight = static_cast<int>(space.size());
    if (gridHeight == 0) return Mesh(vertices, indices);
    int gridWidth = static_cast<int>(space[0].size());

    auto isFloor = [&](int x, int y) -> bool {
        if (x < 0 || y < 0 || x >= gridWidth || y >= gridHeight) return false;
        return space[y][x] != 0;
    };

    auto addQuad = [&](
        float ax, float ay, float az,
        float bx, float by, float bz,
        float cx, float cy, float cz,
        float dx, float dy, float dz,
        float qnx, float qny, float qnz,
        float u0, float v0, float u1, float v1)
    {
        unsigned int vo = static_cast<unsigned int>(vertices.size() / 8);
        vertices.insert(vertices.end(), {
            ax, ay, az, qnx, qny, qnz, u0, v0,
            bx, by, bz, qnx, qny, qnz, u1, v0,
            cx, cy, cz, qnx, qny, qnz, u1, v1,
            dx, dy, dz, qnx, qny, qnz, u0, v1
        });
        indices.insert(indices.end(), {
            vo + 0, vo + 2, vo + 1,
            vo + 2, vo + 0, vo + 3
        });
    };

    auto addWallQuad = [&](float x0, float z0, float x1, float z1)
    {
        float dx = x1 - x0;
        float dz = z1 - z0;
        float wallLength = std::sqrt(dx * dx + dz * dz);
        float u1 = wallLength / wallTextureScale;
        float v1 = wallHeight / wallTextureScale;

        float nx = dz;
        float nz = -dx;
        float invLen = 1.0f / std::sqrt(nx * nx + nz * nz);
        nx *= invLen;
        nz *= invLen;

        addQuad(
            x0, 0.0f,       z0,
            x1, 0.0f,       z1,
            x1, wallHeight, z1,
            x0, wallHeight, z0,
            nx, 0.0f, nz,
            0.0f, 0.0f, u1, v1);
    };

    for (int y = 0; y < gridHeight; ++y)
    {
        for (int x = 0; x < gridWidth; ++x)
        {
            if (!isFloor(x, y)) continue;

            if (!isFloor(x, y - 1))
                addWallQuad((float)x, (float)y, (float)(x + 1), (float)y);
            if (!isFloor(x, y + 1))
                addWallQuad((float)(x + 1), (float)(y + 1), (float)x, (float)(y + 1));
            if (!isFloor(x - 1, y))
                addWallQuad((float)x, (float)(y + 1), (float)x, (float)y);
            if (!isFloor(x + 1, y))
                addWallQuad((float)(x + 1), (float)y, (float)(x + 1), (float)(y + 1));
        }
    }

    return Mesh(vertices, indices, layout);
}

