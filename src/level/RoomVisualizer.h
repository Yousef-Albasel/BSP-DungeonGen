#pragma once
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <cstdint>
#include <algorithm>
#include "../Vendor/stb_image_write.h"

// ─────────────────────────────────────────────────────────────
// RoomVisualizer
// ─────────────────────────────────────────────────────────────
// Takes the 2D grid from BinarySpacePartition (where 0 = wall
// and 1..N = distinct room IDs) and bakes it into a PNG image
// with a unique color per room.
//
// Usage:
//     BinarySpacePartition bsp(50, 50);
//     RoomVisualizer viz;
//     viz.visualize(bsp, "dungeon.png");
//
// Configurable:
//     viz.cellSize      – pixel size of each grid cell  (default 12)
//     viz.wallColor     – RGB for walls                 (default dark grey)
//     viz.saturation    – color saturation 0.0-1.0      (default 0.55)
//     viz.lightness     – color lightness  0.0-1.0      (default 0.60)
//     viz.drawGrid      – draw grid lines between cells (default true)
//     viz.gridColor     – RGB for grid lines            (default #1a1a2e)
// ─────────────────────────────────────────────────────────────

class BinarySpacePartition; // forward declare

struct RGB {
    uint8_t r, g, b;
};

class RoomVisualizer
{
public:
    // --- Configurable parameters ---
    int cellSize       = 12;
    RGB wallColor      = { 22,  22,  46 };    // dark navy
    RGB corridorColor  = { 180, 180, 180 };   // light grey for corridors
    RGB startRoomColor = {  46, 204, 113 };   // vibrant green
    RGB endRoomColor   = { 231,  76,  60 };   // vibrant red
    float saturation   = 0.55f;
    float lightness    = 0.60f;
    bool  drawGrid     = true;
    RGB   gridColor    = { 26,  26,  46 };    // subtle dark line

    // Generate a color for a given room index using the golden
    // angle so that adjacent room IDs are visually distinct.
    RGB roomColor(int roomId, int totalRooms) const
    {
        // Golden angle ≈ 137.508° gives maximally spaced hues
        float hue = std::fmod(roomId * 137.508f, 360.0f);
        return hslToRgb(hue, saturation, lightness);
    }

    // Main entry point — writes a PNG to `outputPath`.
    // Returns true on success.
    bool visualize(const std::vector<std::vector<int>>& grid,
                   int roomCount,
                   int startRoomId,
                   int endRoomId,
                   const std::string& outputPath) const
    {
        if (grid.empty()) return false;

        int gridH = static_cast<int>(grid.size());
        int gridW = static_cast<int>(grid[0].size());

        int imgW = gridW * cellSize;
        int imgH = gridH * cellSize;

        // 3-channel RGB buffer
        std::vector<uint8_t> pixels(imgW * imgH * 3, 0);

        // Pre-compute palette
        std::vector<RGB> palette(roomCount + 1);
        palette[0] = wallColor;
        for (int i = 1; i <= roomCount; i++)
            palette[i] = roomColor(i, roomCount);

        // Fill pixels
        for (int gy = 0; gy < gridH; gy++)
        {
            for (int gx = 0; gx < gridW; gx++)
            {
                int id = grid[gy][gx];
                RGB color;
                if (id == -1) {
                    color = corridorColor;
                } else if (id == startRoomId && id != 0) {
                    color = startRoomColor;
                } else if (id == endRoomId && id != 0) {
                    color = endRoomColor;
                } else if (id > 0 && id <= roomCount) {
                    color = palette[id];
                } else {
                    color = wallColor;
                }

                // Fill the cell
                for (int py = 0; py < cellSize; py++)
                {
                    for (int px = 0; px < cellSize; px++)
                    {
                        int ix = gx * cellSize + px;
                        int iy = gy * cellSize + py;
                        int idx = (iy * imgW + ix) * 3;

                        // Grid lines: draw a 1-pixel border on
                        // the bottom/right edge of each cell
                        if (drawGrid &&
                            (px == cellSize - 1 || py == cellSize - 1))
                        {
                            pixels[idx + 0] = gridColor.r;
                            pixels[idx + 1] = gridColor.g;
                            pixels[idx + 2] = gridColor.b;
                        }
                        else
                        {
                            pixels[idx + 0] = color.r;
                            pixels[idx + 1] = color.g;
                            pixels[idx + 2] = color.b;
                        }
                    }
                }
            }
        }

        // Write PNG via stb_image_write
        int result = stbi_write_png(
            outputPath.c_str(),
            imgW, imgH,
            3,                       // RGB channels
            pixels.data(),
            imgW * 3                 // row stride
        );

        if (result)
            std::cout << "[RoomVisualizer] Saved " << imgW << "x"
                      << imgH << " image to: " << outputPath << "\n";
        else
            std::cerr << "[RoomVisualizer] ERROR: Failed to write "
                      << outputPath << "\n";

        return result != 0;
    }

    // Convenience overload that takes a BSP directly.
    bool visualize(const BinarySpacePartition& bsp,
                   const std::string& outputPath) const;

private:
    // HSL → RGB conversion
    static RGB hslToRgb(float h, float s, float l)
    {
        h = std::fmod(h, 360.0f);
        if (h < 0) h += 360.0f;
        float c = (1.0f - std::fabs(2.0f * l - 1.0f)) * s;
        float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = l - c / 2.0f;

        float r = 0, g = 0, b = 0;
        if      (h < 60)  { r = c; g = x; b = 0; }
        else if (h < 120) { r = x; g = c; b = 0; }
        else if (h < 180) { r = 0; g = c; b = x; }
        else if (h < 240) { r = 0; g = x; b = c; }
        else if (h < 300) { r = x; g = 0; b = c; }
        else              { r = c; g = 0; b = x; }

        return {
            static_cast<uint8_t>(std::clamp((r + m) * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp((g + m) * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp((b + m) * 255.0f, 0.0f, 255.0f))
        };
    }
};
