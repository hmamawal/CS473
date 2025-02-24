#ifndef FOX_HPP
#define FOX_HPP
#include <vector>
#include "Tile.hpp"

struct Fox {
    float x, y;
    float speed = 0.5f;
    int direction = 1; // 1 for right, -1 for left

    void move(const std::vector<Tile>& tiles);
};

#endif // FOX_HPP