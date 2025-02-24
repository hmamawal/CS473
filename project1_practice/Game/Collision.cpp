#include "collision.hpp"

bool checkCollision(float x, float y, float width, float height, const std::vector<Tile>& tiles) {
    for (const auto& tile : tiles) {
        if (tile.isWall) {
            // Same bounding-box test
            if (x < tile.x + TILE_SIZE &&
                x + width > tile.x &&
                y < tile.y + TILE_SIZE &&
                y + height > tile.y) {
                return true;
            }
        }
    }
    return false;
}
