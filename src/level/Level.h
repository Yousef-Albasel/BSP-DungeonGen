#pragma once

#include <iostream>
#include <vector>

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

};