#include "Player.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include "../utilities/constants.hpp"
#include "../utilities/helper.hpp"

Player::Player(float x, float y, float angle, float speed)
    : x(x), y(y), angle(angle), speed(speed) {}

float Player::getX() const { return x; }
float Player::getY() const { return y; }
float Player::getAngle() const { return angle; }
float Player::getSpeed() const { return speed; }

void Player::setX(float x) { this->x = x; }
void Player::setY(float y) { this->y = y; }
void Player::setAngle(float angle) { this->angle = angle; }
void Player::setSpeed(float speed) { this->speed = speed; }

void Player::render(float swingAngle, unsigned int shaderProgram, unsigned int VAO, unsigned int texture) const
{
    // Adjusted dimensions to fit in smaller spaces
    float legHeight = 3.0f, legWidth = 2.0f;
    float bodyHeight = 6.0f, bodyWidth = 6.0f;
    float headRadius = 3.0f;

    // Overall pivot for the player's rotation
    float pivotX = this->x + bodyWidth * 0.5f;
    float pivotY = this->y + (legHeight + bodyHeight + headRadius) * 0.5f;

    // We'll define a helper that draws a rectangle in local coordinates,
    // then rotates it around two pivots (leg pivot, then player's pivot).
    auto drawRectangleWithLegSwing = [&](float x, float y,
                                         float w, float h,
                                         float legPivotX, float legPivotY,
                                         float localAngle)
    {
        float vertices[] = {
            x,      y,      0.0f, 0.0f,
            x + w,  y,      1.0f, 0.0f,
            x + w,  y + h,  1.0f, 1.0f,
            x,      y,      0.0f, 0.0f,
            x + w,  y + h,  1.0f, 1.0f,
            x,      y + h,  0.0f, 1.0f
        };

        // Step 1) Rotate around legPivotX, legPivotY for "leg swing"
        for (int i = 0; i < 6; i++) {
            float& vx = vertices[i*4 + 0];
            float& vy = vertices[i*4 + 1];
            rotatePoint(vx, vy, legPivotX, legPivotY, localAngle);
        }

        // Step 2) Rotate around the player's overall pivot
        for (int i = 0; i < 6; i++) {
            float& vx = vertices[i*4 + 0];
            float& vy = vertices[i*4 + 1];
            rotatePoint(vx, vy, pivotX, pivotY, angle);
        }

        // Now send to GPU
        glBindBuffer(GL_ARRAY_BUFFER, VAO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    };

    //
    // --- DRAW LEGS ---
    //
    float leftLegTopX  = this->x + 1.0f;
    float leftLegTopY  = this->y + legHeight;    
    float rightLegTopX = this->x + 4.0f;
    float rightLegTopY = this->y + legHeight;

    // Left leg (localAngle = +swingAngle)
    drawRectangleWithLegSwing(
        this->x + 1.0f, // lower-left
        this->y,
        legWidth,
        legHeight,
        leftLegTopX,
        leftLegTopY,
        swingAngle
    );
    // Right leg (localAngle = -swingAngle)
    drawRectangleWithLegSwing(
        this->x + 4.0f,
        this->y,
        legWidth,
        legHeight,
        rightLegTopX,
        rightLegTopY,
        -swingAngle
    );

    //
    // --- DRAW BODY ---
    //
    drawRectangleWithLegSwing(
        this->x,
        this->y + legHeight, 
        bodyWidth,
        bodyHeight,
        this->x,  // pivot not really used for the body
        this->y + legHeight,
        0.0f
    );

    //
    // --- DRAW HEAD (a circle) ---
    //
    {
        int segments = 20;
        std::vector<float> circleVertices;
        circleVertices.reserve((segments + 2) * 4);

        float centerX = this->x + bodyWidth / 2.0f;
        float centerY = this->y + legHeight + bodyHeight + headRadius;

        // Center vertex
        circleVertices.push_back(centerX);
        circleVertices.push_back(centerY);
        circleVertices.push_back(0.5f);
        circleVertices.push_back(0.5f);

        for (int i = 0; i <= segments; i++) {
            float angleStep = 2.0f * 3.14159f * i / segments;
            float vx = centerX + headRadius * cos(angleStep);
            float vy = centerY + headRadius * sin(angleStep);
            float tx = (cos(angleStep) + 1.0f) * 0.5f;
            float ty = (sin(angleStep) + 1.0f) * 0.5f;

            // Rotate around the player's pivot
            rotatePoint(vx, vy, pivotX, pivotY, angle);

            circleVertices.push_back(vx);
            circleVertices.push_back(vy);
            circleVertices.push_back(tx);
            circleVertices.push_back(ty);
        }

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindBuffer(GL_ARRAY_BUFFER, VAO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, circleVertices.size() * sizeof(float), circleVertices.data());
        glUseProgram(shaderProgram);
        glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    }
}