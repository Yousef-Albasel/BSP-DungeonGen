#pragma once

#include "Mesh.h"

class Room
{
private:
    int id;
    int x, y;
    int width, height;

public:
    Room(int id, int x, int y, int width, int height)
        : id(id), x(x), y(y), width(width), height(height)
    {
    }

    int getId() const { return id; }

    int getX() const { return x; }
    int getY() const { return y; }

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    Mesh createFloorMesh() const
    {
        std::vector<float> vertices = {
            // x, y, z

            static_cast<float>(x),
            0.0f,
            static_cast<float>(y),

            static_cast<float>(x + width),
            0.0f,
            static_cast<float>(y),

            static_cast<float>(x + width),
            0.0f,
            static_cast<float>(y + height),

            static_cast<float>(x),
            0.0f,
            static_cast<float>(y + height)
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0
        };

        return Mesh(vertices, indices);
    }
};