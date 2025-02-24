#include "Fox.hpp"
#include "Tile.hpp"
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cmath>

const unsigned int TILE_SIZE3 = 20;
const float PLAYER_SIZE3 = 10.0f;

Fox::Fox(float x, float y, float speed, int direction)
    : x(x), y(y), speed(speed), direction(direction) {}

float Fox::getX() const { return x; }
float Fox::getY() const { return y; }
float Fox::getSpeed() const { return speed; }
int Fox::getDirection() const { return direction; }

void Fox::setX(float x) { this->x = x; }
void Fox::setY(float y) { this->y = y; }
void Fox::setSpeed(float speed) { this->speed = speed; }
void Fox::setDirection(int direction) { this->direction = direction; }

void Fox::move(const std::vector<Tile>& tiles) {
    float newX = x + direction * speed;

    // Check for collision with walls
    for (const Tile& tile : tiles) {
        if (tile.isWall) {
            if (newX < tile.x + TILE_SIZE3 &&
                newX + PLAYER_SIZE3 > tile.x &&
                y < tile.y + TILE_SIZE3 &&
                y + PLAYER_SIZE3 > tile.y) {
                direction *= -1; // Change direction
                return;
            }
        }
    }

    x = newX; // Update position if no collision
}

void Fox::render(unsigned int shaderProgram, unsigned int VAO) const {
    // Fox dimensions:
    float legWidth = 3.0f;
    float legHeight = 3.0f;
    float bodyWidth = 22.0f;  // wider horizontal rectangle
    float bodyHeight = 8.0f;
    float headRadius = 4.0f;

    // Lambda to draw a rectangle (two triangles)
    auto drawRectangle = [&](float x, float y, float width, float height) {
        float vertices[] = {
            x,         y,           0.0f, 0.0f,
            x + width, y,           1.0f, 0.0f,
            x + width, y + height,  1.0f, 1.0f,
            x,         y,           0.0f, 0.0f,
            x + width, y + height,  1.0f, 1.0f,
            x,         y + height,  0.0f, 1.0f
        };
        glBindBuffer(GL_ARRAY_BUFFER, VAO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glUseProgram(shaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    };

    // --- Draw 4 Legs ---
    // We choose leg positions relative to fox.x (bottom left of the fox composite shape).
    drawRectangle(x + 1,        y, legWidth, legHeight);
    drawRectangle(x + 7,        y, legWidth, legHeight);
    drawRectangle(x + 13,       y, legWidth, legHeight);
    drawRectangle(x + 19,       y, legWidth, legHeight);

    // --- Draw Body ---
    // Body sits above the legs.
    float bodyX = x;
    float bodyY = y + legHeight;
    drawRectangle(bodyX, bodyY, bodyWidth, bodyHeight);

    // --- Draw Head as a Circle ---
    // Place the head on the left side of the body.
    int segments = 20;
    std::vector<float> circleVertices;
    circleVertices.reserve((segments + 2) * 4); // each vertex: 4 floats
    // Head center: to the left of the body, vertically centered on the body.
    float centerX = bodyX - headRadius;
    float centerY = bodyY + bodyHeight / 2.0f;
    circleVertices.push_back(centerX);
    circleVertices.push_back(centerY);
    circleVertices.push_back(0.5f); // texture coordinate (arbitrary)
    circleVertices.push_back(0.5f);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.14159f * i / segments;
        float vx = centerX + headRadius * cos(angle);
        float vy = centerY + headRadius * sin(angle);
        float tx = (cos(angle) + 1.0f) * 0.5f;
        float ty = (sin(angle) + 1.0f) * 0.5f;
        circleVertices.push_back(vx);
        circleVertices.push_back(vy);
        circleVertices.push_back(tx);
        circleVertices.push_back(ty);
    }
    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, circleVertices.size() * sizeof(float), circleVertices.data());
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
}