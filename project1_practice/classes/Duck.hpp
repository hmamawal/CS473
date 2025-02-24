#ifndef DUCK_HPP
#define DUCK_HPP
#include <vector>
#include "Tile.hpp"

struct Duck {
    float x, y;
    float speed = 0.5f;
    int direction = 1; // 1 for right, -1 for left
    float angle;

    void move(const std::vector<Tile>& tiles);
};

#endif // DUCK_HPP