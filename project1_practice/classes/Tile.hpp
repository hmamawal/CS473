#ifndef TILE_HPP
#define TILE_HPP

class Tile {
public:
    float x, y;
    bool isWall;

    void render(unsigned int shaderProgram, unsigned int VAO, unsigned int texture) const;
};

#endif // TILE_HPP