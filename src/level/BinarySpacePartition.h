#pragma once
#include <vector>
#include <cstdlib>
#include <iostream>
#include "Room.h"


inline int MINIMUM_ROOM_SIZE = 15;
inline int MAX_ROOM_SIZE = 30;
inline int CORRIDOR_WIDTH = 3;

// Gap left as empty (unassigned, value 0) space between two sibling
// partitions after a split. Without this, rooms/corridors from
// neighboring partitions sit flush against each other with zero
// empty cells between them, so generateWallMesh() has nothing to
// detect a boundary against and silently skips the wall there.
inline int PARTITION_GAP = 2;

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

    // The center of one real room somewhere within this partition's
    // subtree. For a leaf this is set to the room's own center. For
    // an internal node it's inherited from one of its children once
    // both are generated. Corridors must connect these points, never
    // getCenter() on an internal node - that only returns the center
    // of the bounding box, which usually lands in the empty gap
    // between children rather than inside any actual room.
    Point connectorPoint;

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

        // Splitting requires room for two child partitions of at
        // least MINIMUM_ROOM_SIZE plus the gap between them.
        bool canSplitVertically =
            width > MINIMUM_ROOM_SIZE * 2 + PARTITION_GAP;

        bool canSplitHorizontally =
            height > MINIMUM_ROOM_SIZE * 2 + PARTITION_GAP;

        if (!canSplitVertically && !canSplitHorizontally)
        {
            AssignRoom(partition);
            return;
        }

        // Split vertically
        if (canSplitVertically && !canSplitHorizontally)
        {
            int split =
                rand() % (width - MINIMUM_ROOM_SIZE * 2 - PARTITION_GAP)
                + MINIMUM_ROOM_SIZE;

            partition->left = new Partition(
                partition->x,
                partition->y,
                split,
                height
            );

            partition->right = new Partition(
                partition->x + split + PARTITION_GAP,
                partition->y,
                width - split - PARTITION_GAP,
                height
            );
        }

        // Split horizontally
        else if (!canSplitVertically && canSplitHorizontally)
        {
            int split =
                rand() % (height - MINIMUM_ROOM_SIZE * 2 - PARTITION_GAP)
                + MINIMUM_ROOM_SIZE;

            partition->left = new Partition(
                partition->x,
                partition->y,
                width,
                split
            );

            partition->right = new Partition(
                partition->x,
                partition->y + split + PARTITION_GAP,
                width,
                height - split - PARTITION_GAP
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
                    rand() % (width - MINIMUM_ROOM_SIZE * 2 - PARTITION_GAP)
                    + MINIMUM_ROOM_SIZE;

                partition->left = new Partition(
                    partition->x,
                    partition->y,
                    split,
                    height
                );

                partition->right = new Partition(
                    partition->x + split + PARTITION_GAP,
                    partition->y,
                    width - split - PARTITION_GAP,
                    height
                );
            }

            // Horizontal
            else
            {
                int split =
                    rand() % (height - MINIMUM_ROOM_SIZE * 2 - PARTITION_GAP)
                    + MINIMUM_ROOM_SIZE;

                partition->left = new Partition(
                    partition->x,
                    partition->y,
                    width,
                    split
                );

                partition->right = new Partition(
                    partition->x,
                    partition->y + split + PARTITION_GAP,
                    width,
                    height - split - PARTITION_GAP
                );
            }
        }

        generateRooms(partition->left);
        generateRooms(partition->right);

        // Inherit a real room's center from one of the children so
        // any corridor connecting to *this* partition (from further
        // up the tree) still terminates inside an actual room rather
        // than in the empty gap between its children.
        partition->connectorPoint =
            (rand() % 2 == 0)
                ? partition->left->connectorPoint
                : partition->right->connectorPoint;
    }

    void drawCorridor(Point p1, Point p2)
    {
        int halfWidth = CORRIDOR_WIDTH / 2;

        auto carve = [&](int cx, int cy)
        {
            for (int dy = -halfWidth; dy <= halfWidth; ++dy)
            {
                for (int dx = -halfWidth; dx <= halfWidth; ++dx)
                {
                    int nx = cx + dx;
                    int ny = cy + dy;
                    if (ny < 0 || ny >= (int)space.size()) continue;
                    if (nx < 0 || nx >= (int)space[0].size()) continue;
                    if (space[ny][nx] == 0)
                        space[ny][nx] = -1;
                }
            }
        };

        int x = p1.x;
        int y = p1.y;

        while (x != p2.x) {
            carve(x, y);
            x += (p2.x > p1.x) ? 1 : -1;
        }
        while (y != p2.y) {
            carve(x, y);
            y += (p2.y > p1.y) ? 1 : -1;
        }
        carve(p2.x, p2.y);
    }

    void generateCorridors(Partition* partition)
    {
        if (partition == nullptr || (partition->left == nullptr && partition->right == nullptr))
            return;

        generateCorridors(partition->left);
        generateCorridors(partition->right);

        Point p1 = partition->left->connectorPoint;
        Point p2 = partition->right->connectorPoint;

        drawCorridor(p1, p2);
    }

    // Each leaf partition gets a unique room ID (1, 2, 3, ...)
    // so the visualizer can assign a distinct color per room.
    void drawPartition(Partition* partition)
    {
        if (partition == nullptr)
            return;

        if (partition->left == nullptr && partition->right == nullptr)
        {
            int roomId = partition->roomId; // set earlier in AssignRoom

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
        partition->connectorPoint = partition->getCenter(); // real room center
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

    }

    // --- Accessors for the visualizer ---
    const std::vector<std::vector<int>>& getSpace() const { return space; }
    int getRoomCount() const { return roomCount; }
    int getStartRoomId() const { return 1 > roomCount ? 0 : 1; }
    int getEndRoomId() const { return roomCount; }
    int getWidth()  const { return spaceWidth; }
    int getHeight() const { return spaceHeight; }
};