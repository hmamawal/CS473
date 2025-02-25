#include "Tile.hpp"
#include <glad/glad.h>
#include "../utilities/constants.hpp"

void Tile::render(unsigned int shaderProgram, unsigned int VAO, unsigned int texture) const {
    if (isWall) {
        float vertices[] = {
            x, y,                     0.0f, 0.0f,
            x + TILE_SIZE, y,         1.0f, 0.0f,
            x + TILE_SIZE, y + TILE_SIZE, 1.0f, 1.0f,
            x, y,                     0.0f, 0.0f,
            x + TILE_SIZE, y + TILE_SIZE, 1.0f, 1.0f,
            x, y + TILE_SIZE,         0.0f, 1.0f
        };
        glBindBuffer(GL_ARRAY_BUFFER, VAO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}