#include "Fox.hpp"
#include "Tile.hpp"
#include <vector>
#include "Tile.hpp"
const unsigned int TILE_SIZE3 = 20;
const float PLAYER_SIZE3 = 10.0f;

void Fox::move(const std::vector<Tile>& tiles) {
    float newX = x + direction * speed;

    // Check for collision with walls
    for (const Tile& tile : tiles) {
        if (tile.isWall) {
            if (newX < tile.x + TILE_SIZE3 &&
                newX + PLAYER_SIZE3 > tile.x &&
                y < tile.y + TILE_SIZE3 &&
                y + PLAYER_SIZE3 > tile.y) {
                direction *= -1; // Change direction
                return;
            }
        }
    }

    x = newX; // Update position if no collision
}