#include "Heart.hpp"
#include <glad/glad.h>

Heart::Heart(float x, float y, bool isFull)
    : x(x), y(y), full(isFull) {}

float Heart::getX() const { return x; }
float Heart::getY() const { return y; }
bool Heart::isFull() const { return full; }

void Heart::setX(float x) { this->x = x; }
void Heart::setY(float y) { this->y = y; }
void Heart::setFull(bool isFull) { this->full = isFull; }

void Heart::render(unsigned int shaderProgram, unsigned int VAO, unsigned int fullTexture, unsigned int emptyTexture) const {
    float heartSize = 20.0f;
    unsigned int textureToUse = full ? fullTexture : emptyTexture;
    float vertices[] = {
        x, y,                      0.0f, 0.0f,
        x + heartSize, y,          1.0f, 0.0f,
        x + heartSize, y + heartSize, 1.0f, 1.0f,
        x, y,                      0.0f, 0.0f,
        x + heartSize, y + heartSize, 1.0f, 1.0f,
        x, y + heartSize,          0.0f, 1.0f
    };
    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glUseProgram(shaderProgram);
    glBindTexture(GL_TEXTURE_2D, textureToUse);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}