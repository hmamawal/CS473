#include "Fox.hpp"
#include "Tile.hpp"
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cmath>
#include "../utilities/constants.hpp"
#include "../utilities/helper.hpp"

Fox::Fox(float x, float y, float speed, int direction, float angle)
    : x(x), y(y), speed(speed), direction(direction), angle(angle) {}

float Fox::getX() const { return x; }
float Fox::getY() const { return y; }
float Fox::getSpeed() const { return speed; }
int Fox::getDirection() const { return direction; }
float Fox::getAngle() const { return angle; }

void Fox::setX(float x) { this->x = x; }
void Fox::setY(float y) { this->y = y; }
void Fox::setSpeed(float speed) { this->speed = speed; }
void Fox::setDirection(int direction) { this->direction = direction; }
void Fox::setAngle(float angle) { this->angle = angle; }

void Fox::move(const std::vector<Tile>& tiles) {
    // Attempt moving left or right.
    float newX = x + direction * speed;

    // Check collision with walls:
    for (const Tile& tile : tiles) {
        if (tile.isWall) {
            // If colliding, flip direction and update angle, then stop moving.
            if (newX < tile.x + TILE_SIZE &&
                newX + ENTITY_SIZE > tile.x &&
                y < tile.y + TILE_SIZE &&
                y + ENTITY_SIZE > tile.y) {
                direction *= -1;
                angle = (direction == 1) ? 0.0f : 180.0f;
                return;
            }
        }
    }

    // If no collision, update position.
    x = newX;

    // Update the angle to face the direction of movement
    angle = (direction == 1) ? 0.0f : 180.0f;

    // Update leg swing angle if the fox is moving
    legSwingAngle += LEG_SWING_SPEED * legSwingDirection;
    if (legSwingAngle > 30.0f || legSwingAngle < -30.0f) {
        legSwingDirection *= -1.0f;
    }
}

void Fox::render(unsigned int shaderProgram, unsigned int VAO, unsigned int texture) const {
    //
    // Dimensions
    //
    float legWidth   = 3.0f;
    float legHeight  = 3.0f;
    float bodyWidth  = 22.0f;
    float bodyHeight = 8.0f;
    float headRadius = 4.0f;

    //
    // We define a single pivot around which the entire fox will be rotated (angle).
    // This pivot is roughly in the center of the Fox’s bounding box.
    //
    float pivotX = x + bodyWidth * 0.5f; 
    float pivotY = y + (legHeight + bodyHeight + headRadius) * 0.5f;

    //
    // This helper function rotates a rectangle around:
    //   (1) a local “leg pivot,” for leg swinging,
    //   (2) the overall fox pivot, for facing left/right.
    //
    auto drawRectangleWithLegSwing = [&](float rectX, float rectY,
                                         float w, float h,
                                         float legPivotX, float legPivotY,
                                         float localLegAngle)
    {
        float vertices[] = {
            rectX,       rectY,        0.0f, 0.0f,
            rectX + w,   rectY,        1.0f, 0.0f,
            rectX + w,   rectY + h,    1.0f, 1.0f,
            rectX,       rectY,        0.0f, 0.0f,
            rectX + w,   rectY + h,    1.0f, 1.0f,
            rectX,       rectY + h,    0.0f, 1.0f
        };

        // 1) Rotate around the local leg pivot for the swing
        for (int i = 0; i < 6; i++) {
            float& vx = vertices[i*4 + 0];
            float& vy = vertices[i*4 + 1];
            rotatePoint(vx, vy, legPivotX, legPivotY, localLegAngle);
        }
        // 2) Rotate the result around the overall fox pivot for facing direction
        for (int i = 0; i < 6; i++) {
            float& vx = vertices[i*4 + 0];
            float& vy = vertices[i*4 + 1];
            rotatePoint(vx, vy, pivotX, pivotY, angle);
        }

        // Draw it
        glBindBuffer(GL_ARRAY_BUFFER, VAO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    };

    //
    // --- Draw each leg. ---
    // We'll define their top pivot so the leg swings at its top attach point.
    //
    // Leg #1 (front left)
    {
        float legX = x + 1.0f; 
        float legY = y; 
        float legPivotX = legX + legWidth * 0.5f;      // pivot at top center
        float legPivotY = legY + legHeight; 
        // localLegAngle = +legSwingAngle
        drawRectangleWithLegSwing(legX, legY, legWidth, legHeight, 
                                  legPivotX, legPivotY,  legSwingAngle);
    }
    // Leg #2 (front right)
    {
        float legX = x + 7.0f;  
        float legY = y;
        float legPivotX = legX + legWidth * 0.5f;
        float legPivotY = legY + legHeight;
        // localLegAngle = -legSwingAngle
        drawRectangleWithLegSwing(legX, legY, legWidth, legHeight, 
                                  legPivotX, legPivotY, -legSwingAngle);
    }
    // Leg #3 (rear left)
    {
        float legX = x + 13.0f; 
        float legY = y; 
        float legPivotX = legX + legWidth * 0.5f;
        float legPivotY = legY + legHeight;
        drawRectangleWithLegSwing(legX, legY, legWidth, legHeight, 
                                  legPivotX, legPivotY,  legSwingAngle);
    }
    // Leg #4 (rear right)
    {
        float legX = x + 19.0f; 
        float legY = y; 
        float legPivotX = legX + legWidth * 0.5f;
        float legPivotY = legY + legHeight;
        drawRectangleWithLegSwing(legX, legY, legWidth, legHeight, 
                                  legPivotX, legPivotY, -legSwingAngle);
    }

    //
    // --- Draw Body ---
    //  We won't swing the body, so localLegAngle = 0. We'll still rotate around
    //  the fox pivot for left/right orientation.
    //
    {
        float bodyX = x;
        float bodyY = y + legHeight;
        float w = bodyWidth, h = bodyHeight;

        // We'll pick a "dummy" pivot for the leg pivot call (body won't swing),
        // but then it still gets rotated around the overall pivot at the final step.
        float dummyPivotX = bodyX; 
        float dummyPivotY = bodyY;
        drawRectangleWithLegSwing(bodyX, bodyY, w, h, dummyPivotX, dummyPivotY, 0.0f);
    }

    //
    // --- Draw Head (a circle) ---
    //   We rotate all circle points around the overall pivot by 'angle.'
    //   We do *not* do a “leg pivot” for the head, so no localLegAngle. 
    //
    {
        int   segments = 20;
        std::vector<float> circleVertices;
        circleVertices.reserve((segments + 2) * 4);

        float bodyX = x;
        float bodyY = y + legHeight;
        float centerX = bodyX - headRadius;            // left of the body
        float centerY = bodyY + bodyHeight * 0.5f;     // center in Y

        // First push the center vertex (for TRIANGLE_FAN).
        circleVertices.push_back(centerX);
        circleVertices.push_back(centerY);
        circleVertices.push_back(0.5f);
        circleVertices.push_back(0.5f);

        // Then push each segment around the circle
        for (int i = 0; i <= segments; ++i) {
            float angleStep = 2.0f * 3.14159f * i / segments;
            float vx = centerX + headRadius * cos(angleStep);
            float vy = centerY + headRadius * sin(angleStep);

            float tx = (cos(angleStep) + 1.0f) * 0.5f;  // basic circular UV
            float ty = (sin(angleStep) + 1.0f) * 0.5f;

            // Rotate each point around the overall fox pivot
            rotatePoint(vx, vy, pivotX, pivotY, angle);

            circleVertices.push_back(vx);
            circleVertices.push_back(vy);
            circleVertices.push_back(tx);
            circleVertices.push_back(ty);
        }

        glBindBuffer(GL_ARRAY_BUFFER, VAO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, circleVertices.size() * sizeof(float), circleVertices.data());
        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    }
}
