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
    float newX = x + direction * speed;

    // Check for collision with walls
    for (const Tile& tile : tiles) {
        if (tile.isWall) {
            if (newX < tile.x + TILE_SIZE &&
                newX + ENTITY_SIZE > tile.x &&
                y < tile.y + TILE_SIZE &&
                y + ENTITY_SIZE > tile.y) {
                direction *= -1; // Change direction
                angle = (direction == 1) ? 0.0f : 180.0f; // Update angle based on direction
                return;
            }
        }
    }

    x = newX; // Update position if no collision

    // Update leg swing angle if the fox is moving
    legSwingAngle += LEG_SWING_SPEED * legSwingDirection;
    if (legSwingAngle > 30.0f || legSwingAngle < -30.0f) {
        legSwingDirection *= -1.0f;
    }
}

void Fox::render(unsigned int shaderProgram, unsigned int VAO, unsigned int texture) const {
    // Fox dimensions:
    float legWidth = 3.0f;
    float legHeight = 3.0f;
    float bodyWidth = 22.0f;  // wider horizontal rectangle
    float bodyHeight = 8.0f;
    float headRadius = 4.0f;

    // Overall pivot for the fox's rotation
    float pivotX = this->x + bodyWidth * 0.5f;
    float pivotY = this->y + (legHeight + bodyHeight + headRadius) * 0.5f;

    // Lambda to draw a rectangle with rotation
    auto drawRectangleWithRotation = [&](float x, float y, float width, float height, float angle) {
        float vertices[] = {
            x,         y,           0.0f, 0.0f,
            x + width, y,           1.0f, 0.0f,
            x + width, y + height,  1.0f, 1.0f,
            x,         y,           0.0f, 0.0f,
            x + width, y + height,  1.0f, 1.0f,
            x,         y + height,  0.0f, 1.0f
        };

        // Rotate around the pivot
        for (int i = 0; i < 6; i++) {
            float& vx = vertices[i*4 + 0];
            float& vy = vertices[i*4 + 1];
            rotatePoint(vx, vy, pivotX, pivotY, angle);
        }

        glBindBuffer(GL_ARRAY_BUFFER, VAO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    };

    // --- Draw Legs ---
    drawRectangleWithRotation(x + 1,        y, legWidth, legHeight, legSwingAngle);
    drawRectangleWithRotation(x + 7,        y, legWidth, legHeight, -legSwingAngle);
    drawRectangleWithRotation(x + 13,       y, legWidth, legHeight, legSwingAngle);
    drawRectangleWithRotation(x + 19,       y, legWidth, legHeight, -legSwingAngle);

    // --- Draw Body ---
    float bodyX = x;
    float bodyY = y + legHeight;
    drawRectangleWithRotation(bodyX, bodyY, bodyWidth, bodyHeight, angle);

    // --- Draw Head as a Circle ---
    int segments = 20;
    std::vector<float> circleVertices;
    circleVertices.reserve((segments + 2) * 4);

    float centerX = bodyX - headRadius;
    float centerY = bodyY + bodyHeight / 2.0f;
    circleVertices.push_back(centerX);
    circleVertices.push_back(centerY);
    circleVertices.push_back(0.5f);
    circleVertices.push_back(0.5f);
    for (int i = 0; i <= segments; ++i) {
        float angleStep = 2.0f * 3.14159f * i / segments;
        float vx = centerX + headRadius * cos(angleStep);
        float vy = centerY + headRadius * sin(angleStep);
        float tx = (cos(angleStep) + 1.0f) * 0.5f;
        float ty = (sin(angleStep) + 1.0f) * 0.5f;
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