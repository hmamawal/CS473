#include "helper.hpp"
void rotatePoint(float& px, float& py, float cx, float cy, float angleDeg)
{
    float rad = glm::radians(angleDeg);
    float tx = px - cx;
    float ty = py - cy;

    float cosA = cos(rad);
    float sinA = sin(rad);

    float rx = tx * cosA - ty * sinA;
    float ry = tx * sinA + ty * cosA;

    px = rx + cx;
    py = ry + cy;
}