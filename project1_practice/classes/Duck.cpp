#include "Duck.hpp"
#include "Tile.hpp"
#include <vector>
#include "Tile.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cmath>

const unsigned int TILE_SIZE2 = 20;
const float PLAYER_SIZE2 = 10.0f;

void Duck::move(const std::vector<Tile>& tiles) {
    float newX = x + direction * speed;

    // Check for collision with walls
    for (const Tile& tile : tiles) {
        if (tile.isWall) {
            if (newX < tile.x + TILE_SIZE2 &&
                newX + PLAYER_SIZE2 > tile.x &&
                y < tile.y + TILE_SIZE2 &&
                y + PLAYER_SIZE2 > tile.y) {
                direction *= -1; // Change direction
                return;
            }
        }
    }

    x = newX; // Update position if no collision
}

float Duck::getX() const { return x; }
float Duck::getY() const { return y; }
void Duck::setX(float x) { this->x = x; }
void Duck::setY(float y) { this->y = y; }

// Render the duck as a composite shape: a circular body, a circular head, and two triangular wings.
void Duck::render(unsigned int shaderProgram, unsigned int VAO) const {
    // Duck dimensions:
    float bodyRadius = 5.0f; // Body circle (diameter = 10)
    float headRadius = 3.0f; // Head circle
    int segments = 20;       // For circle approximations

    // --- Draw Body as a Circle ---
    std::vector<float> bodyVertices;
    bodyVertices.reserve((segments + 2) * 4);
    // Assume duck.x, duck.y is the bottom left of the duck's composite shape.
    float bodyCenterX = x + bodyRadius;
    float bodyCenterY = y + bodyRadius;
    bodyVertices.push_back(bodyCenterX);
    bodyVertices.push_back(bodyCenterY);
    bodyVertices.push_back(0.5f);
    bodyVertices.push_back(0.5f);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.14159f * i / segments;
        float vx = bodyCenterX + bodyRadius * cos(angle);
        float vy = bodyCenterY + bodyRadius * sin(angle);
        float tx = (cos(angle) + 1.0f) * 0.5f;
        float ty = (sin(angle) + 1.0f) * 0.5f;
        bodyVertices.push_back(vx);
        bodyVertices.push_back(vy);
        bodyVertices.push_back(tx);
        bodyVertices.push_back(ty);
    }
    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, bodyVertices.size() * sizeof(float), bodyVertices.data());
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);

    // --- Draw Head as a Circle ---
    std::vector<float> headVertices;
    headVertices.reserve((segments + 2) * 4);
    // Place the head to the right side of the body.
    float headCenterX = x + 2 * bodyRadius + headRadius; // e.g., duck.x + 10 + 3 = duck.x + 13
    float headCenterY = y + bodyRadius;
    headVertices.push_back(headCenterX);
    headVertices.push_back(headCenterY);
    headVertices.push_back(0.5f);
    headVertices.push_back(0.5f);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.14159f * i / segments;
        float vx = headCenterX + headRadius * cos(angle);
        float vy = headCenterY + headRadius * sin(angle);
        float tx = (cos(angle) + 1.0f) * 0.5f;
        float ty = (sin(angle) + 1.0f) * 0.5f;
        headVertices.push_back(vx);
        headVertices.push_back(vy);
        headVertices.push_back(tx);
        headVertices.push_back(ty);
    }
    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, headVertices.size() * sizeof(float), headVertices.data());
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);

    // --- Draw Wings as Triangles ---
    // Define a top wing
    float topWingVertices[] = {
        bodyCenterX,                bodyCenterY,                0.0f, 0.0f,
        bodyCenterX - 3.0f,         bodyCenterY + bodyRadius,   1.0f, 0.0f,
        bodyCenterX + 2.0f,         bodyCenterY + bodyRadius/2, 0.5f, 1.0f
    };
    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(topWingVertices), topWingVertices);
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Define a bottom wing
    float bottomWingVertices[] = {
        bodyCenterX,                bodyCenterY,                0.0f, 0.0f,
        bodyCenterX - 3.0f,         bodyCenterY - bodyRadius,   1.0f, 0.0f,
        bodyCenterX + 2.0f,         bodyCenterY - bodyRadius/2, 0.5f, 1.0f
    };
    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(bottomWingVertices), bottomWingVertices);
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}