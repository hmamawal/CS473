#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <vector>
#include "../classes/Tile.hpp"
#include "../utilities/constants.hpp"

bool checkCollision(float x, float y, float width, float height, const std::vector<Tile>& tiles);

#endif // COLLISION_HPP