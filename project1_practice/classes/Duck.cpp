#include "Duck.hpp"
#include "Tile.hpp"
#include <vector>
#include "Tile.hpp"
const unsigned int TILE_SIZE2 = 20;
const float PLAYER_SIZE2 = 10.0f;

void Duck::move(const std::vector<Tile>& tiles) {
    float newX = x + direction * speed;

    // Check for collision with walls
    for (const Tile& tile : tiles) {
        if (tile.isWall) {
            if (newX < tile.x + TILE_SIZE2 &&
                newX + PLAYER_SIZE2 > tile.x &&
                y < tile.y + TILE_SIZE2 &&
                y + PLAYER_SIZE2 > tile.y) {
                direction *= -1; // Change direction
                return;
            }
        }
    }

    x = newX; // Update position if no collision
}