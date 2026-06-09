#pragma once
#include "BaseObject.h"
#include <cmath>

class Ground : public BaseObject
{
private:
    GroundMode mode;
    bool isBarrier;
    C2D_SpriteSheet platformEdgeSheet, platformTileSheet;
    C2D_Image platformEdgeImg, platformTileImg;
    float radius = 0.0f;

public:
    Ground(float x, float y, float height, float width, bool isBarrier, GroundMode mode)
        : BaseObject(x, y, height, width), mode(mode), isBarrier(isBarrier) {}
    Ground(float x, float y, float height, float width, bool isBarrier, GroundMode mode, float radius, float speed = 0.0f)
        : BaseObject(x, y, height, width, speed), mode(mode), isBarrier(isBarrier), radius(radius)
    {
        platformEdgeSheet = C2D_SpriteSheetLoad("romfs:/gfx/platformedge.t3x");
        platformTileSheet = C2D_SpriteSheetLoad("romfs:/gfx/platformtile.t3x");
        platformEdgeImg = C2D_SpriteSheetGetImage(platformEdgeSheet, 0);
        platformTileImg = C2D_SpriteSheetGetImage(platformTileSheet, 0);

        switch (mode)
        {
        case GroundMode::Horizontal:
            vx = speed;
            break;
        case GroundMode::Vertical:
            vy = -speed;
            break;
        case GroundMode::Descent:
            vx = speed;
            vy = -speed;
            break;
        case GroundMode::Rise:
            vx = -speed;
            vy = -speed;
            break;
        default:
            break;
        }
    }

    void moveLeft() { changeX(-getSpeed()); }
    void moveRight() { changeX(getSpeed()); }

    void draw(float cameraPos, int layer)
    {
        float drawX = x - cameraPos;

        constexpr float EDGE_W = 20.0f;
        constexpr float TILE_W = 3.0f;

        // Platform length 20 or less -> only left edge
        if (width <= EDGE_W)
        {
            C2D_DrawImageAt(platformEdgeImg, drawX, y, layer);
            return;
        }

        // Platform length 21..39 -> left edge + middle tiles
        if (width < EDGE_W * 2.0f)
        {
            C2D_DrawImageAt(platformEdgeImg, drawX, y, layer);

            for (float tx = EDGE_W; tx < width; tx += TILE_W)
            {
                C2D_DrawImageAt(
                    platformTileImg,
                    drawX + tx,
                    y,
                    layer
                );
            }

            return;
        }

        // Draw left edge
        C2D_DrawImageAt(platformEdgeImg, drawX, y, layer);

        // Draw middle section
        for (float tx = EDGE_W; tx < width - EDGE_W; tx += TILE_W)
        {
            C2D_DrawImageAt(
                platformTileImg,
                drawX + tx,
                y,
                layer
            );
        }

        // Draw right edge (flipped left edge)
        C2D_DrawImageAt(
            platformEdgeImg,
            drawX + width - EDGE_W,
            y,
            layer,
            nullptr,
            -1.0f,
            1.0f
        );
    }

    using BaseObject::update;
    void update(uint64_t timer)
    {
        switch (mode)
        {
        case GroundMode::Horizontal:
            // x = spawnX + radius * std::sinf(timer * omega);
            // vx = x - prevX;
            if (x >= spawnX + radius || x <= spawnX - radius)
                vx *= -1.0f;
            break;
        case GroundMode::Vertical:
            // y = spawnY + radius * std::cosf(timer * omega);
            // vy = y - prevY;
            if (y >= spawnY + radius || y <= spawnY - radius)
                vy *= -1.0f;
            break;
        case GroundMode::Descent:
        {
            float radiusXY = std::sqrt(std::pow(radius, 2) / 2);
            if (x >= spawnX + radiusXY || x <= spawnX - radiusXY)
                vx *= -1.0f;
            if (y >= spawnY + radiusXY || y <= spawnY - radiusXY)
                vy *= -1.0f;
            break;
        }
        case GroundMode::Rise:
        {
            float radiusXY = std::sqrt(std::pow(radius, 2) / 2);
            if (x >= spawnX + radiusXY || x <= spawnX - radiusXY)
                vx *= -1.0f;
            if (y >= spawnY + radiusXY || y <= spawnY - radiusXY)
                vy *= -1.0f;
            break;
        }
        default:
            break;
        }
        x += vx;
        y -= vy;
    }
    void freeSheets() {
        C2D_SpriteSheetFree(platformEdgeSheet);
        C2D_SpriteSheetFree(platformTileSheet);
    }
    bool getIsBarrier() { return isBarrier; }
};