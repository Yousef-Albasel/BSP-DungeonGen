#pragma once
#include <vector>
#include <cstdlib>
#include <iostream>
#include "Room.h"


inline int MINIMUM_ROOM_SIZE = 7;
inline int MAX_ROOM_SIZE = 10;
struct Point {
    int x, y;
    Point() : x(0), y(0) {}
    Point(int x, int y) : x(x), y(y) {}
};

struct Partition
{
    int x, y, width, height;

    Partition* left;
    Partition* right;
    int roomId = -1;

    Partition(int x, int y, int width, int height)
        : x(x),
          y(y),
          width(width),
          height(height),
          left(nullptr),
          right(nullptr)
    {
    }

    Point getCenter() const {
        return Point(x + width / 2, y + height / 2);
    }
};

class BinarySpacePartition
{
private:
    std::vector<std::vector<int>> space;
    int spaceWidth;
    int spaceHeight;
    int roomCount = 0;
    std::vector<Room>& rooms;

    void generateRooms(Partition* partition)
    {
        int width = partition->width;
        int height = partition->height;

        bool canSplitVertically =
            width > MINIMUM_ROOM_SIZE * 2;

        bool canSplitHorizontally =
            height > MINIMUM_ROOM_SIZE * 2;

        if (!canSplitVertically && !canSplitHorizontally)
        {
            AssignRoom(partition);
            return;
        }

        // Split vertically
        if (canSplitVertically && !canSplitHorizontally)
        {
            int split =
                rand() % (width - MINIMUM_ROOM_SIZE * 2)
                + MINIMUM_ROOM_SIZE;

            partition->left = new Partition(
                partition->x,
                partition->y,
                split,
                height
            );

            partition->right = new Partition(
                partition->x + split,
                partition->y,
                width - split,
                height
            );
        }

        // Split horizontally
        else if (!canSplitVertically && canSplitHorizontally)
        {
            int split =
                rand() % (height - MINIMUM_ROOM_SIZE * 2)
                + MINIMUM_ROOM_SIZE;

            partition->left = new Partition(
                partition->x,
                partition->y,
                width,
                split
            );

            partition->right = new Partition(
                partition->x,
                partition->y + split,
                width,
                height - split
            );
        }

        // Both directions are possible
        else
        {
            int randomDirection = rand() % 2;

            // Vertical
            if (randomDirection == 0)
            {
                int split =
                    rand() % (width - MINIMUM_ROOM_SIZE * 2)
                    + MINIMUM_ROOM_SIZE;

                partition->left = new Partition(
                    partition->x,
                    partition->y,
                    split,
                    height
                );

                partition->right = new Partition(
                    partition->x + split,
                    partition->y,
                    width - split,
                    height
                );
            }

            // Horizontal
            else
            {
                int split =
                    rand() % (height - MINIMUM_ROOM_SIZE * 2)
                    + MINIMUM_ROOM_SIZE;

                partition->left = new Partition(
                    partition->x,
                    partition->y,
                    width,
                    split
                );

                partition->right = new Partition(
                    partition->x,
                    partition->y + split,
                    width,
                    height - split
                );
            }
        }

        generateRooms(partition->left);
        generateRooms(partition->right);
    }

    void drawCorridor(Point p1, Point p2)
    {
        int x = p1.x;
        int y = p1.y;

        while (x != p2.x) {
            if (space[y][x] == 0) space[y][x] = -1;
            x += (p2.x > p1.x) ? 1 : -1;
        }
        while (y != p2.y) {
            if (space[y][x] == 0) space[y][x] = -1;
            y += (p2.y > p1.y) ? 1 : -1;
        }
        if (space[p2.y][p2.x] == 0) space[p2.y][p2.x] = -1;
    }

    void generateCorridors(Partition* partition)
    {
        if (partition == nullptr || (partition->left == nullptr && partition->right == nullptr))
            return;

        generateCorridors(partition->left);
        generateCorridors(partition->right);

        Point p1 = partition->left->getCenter();
        Point p2 = partition->right->getCenter();

        drawCorridor(p1, p2);
    }

    // Each leaf partition gets a unique room ID (1, 2, 3, ...)
    // so the visualizer can assign a distinct color per room.
    void drawPartition(Partition* partition)
    {
        if (partition == nullptr)
            return;

        // Only draw leaf partitions
        if (partition->left == nullptr &&
            partition->right == nullptr)
        {
            roomCount++;
            int roomId = roomCount;

            // Inset by 1 cell so neighboring rooms don't visually
            // merge into a single solid block. This is purely a
            // rendering choice - the partition tree itself is untouched.
            for (int y = partition->y + 1;
                 y < partition->y + partition->height - 1;
                 y++)
            {
                for (int x = partition->x + 1;
                     x < partition->x + partition->width - 1;
                     x++)
                {
                    space[y][x] = roomId;
                }
            }

            return;
        }

        drawPartition(partition->left);
        drawPartition(partition->right);
    }
    void AssignRoom(Partition* partition)
    {
        roomCount++;

        Room newRoom(
            roomCount,
            partition->x,
            partition->y,
            partition->width,
            partition->height
        );

        rooms.push_back(newRoom);

        partition->roomId = roomCount;
    }

    void printSpace()
    {
        for (const auto& row : space)
        {
            for (int cell : row)
            {
                if (cell > 0)
                    std::cout << ".";
                else
                    std::cout << "#";
            }

            std::cout << '\n';
        }
    }

public:
    BinarySpacePartition(int width, int height, std::vector<Room>& rooms)
        : spaceWidth(width), spaceHeight(height), roomCount(0), rooms(rooms)
    {
        space.resize(height, std::vector<int>(width, 0));

        Partition* root = new Partition(
            0,
            0,
            width,
            height
        );

        generateRooms(root);

        drawPartition(root);
        generateCorridors(root);

        printSpace();
    }

    // --- Accessors for the visualizer ---
    const std::vector<std::vector<int>>& getSpace() const { return space; }
    int getRoomCount() const { return roomCount; }
    int getStartRoomId() const { return 1 > roomCount ? 0 : 1; }
    int getEndRoomId() const { return roomCount; }
    int getWidth()  const { return spaceWidth; }
    int getHeight() const { return spaceHeight; }
};