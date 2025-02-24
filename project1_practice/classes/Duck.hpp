#ifndef DUCK_HPP
#define DUCK_HPP
#include <vector>
#include "Tile.hpp"

class Duck {
public:
    Duck() : x(0), y(0), speed(0.5f), direction(1), angle(0) {}

    void move(const std::vector<Tile>& tiles);

    // Render the duck as a composite shape: a circular body, a circular head, and two triangular wings.
    void render(unsigned int shaderProgram, unsigned int VAO, unsigned int texture) const;

    float getX() const;
    float getY() const;

    void setX(float x);
    void setY(float y);

private:
    float x, y;
    float speed;
    int direction; // 1 for right, -1 for left
    float angle;
};

#endif // DUCK_HPP