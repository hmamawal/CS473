#ifndef FOX_HPP
#define FOX_HPP

#include <vector>
#include "Tile.hpp"

class Fox {
public:
    Fox(float x = 0.0f, float y = 0.0f, float speed = 0.5f, int direction = 1, float angle = 0.0f);

    float getX() const;
    float getY() const;
    float getSpeed() const;
    int getDirection() const;
    float getAngle() const;

    void setX(float x);
    void setY(float y);
    void setSpeed(float speed);
    void setDirection(int direction);
    void setAngle(float angle);

    void move(const std::vector<Tile>& tiles);
    void render(unsigned int shaderProgram, unsigned int VAO) const;

private:
    float x, y;
    float speed;
    int direction;
    float angle;
    float legSwingAngle = 0.0f;
    float legSwingDirection = 1.0f;
};

#endif // FOX_HPP