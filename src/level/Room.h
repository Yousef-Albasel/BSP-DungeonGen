#pragma once


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


};