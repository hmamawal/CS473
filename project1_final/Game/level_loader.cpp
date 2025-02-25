// level_loader.cpp
#include "level_loader.hpp"
#include <fstream>
#include <iostream>

std::vector<Tile> loadMap(const std::string& filename, Player& player,
                          std::vector<Duck>& ducks, std::vector<Fox>& foxes)
{
    std::vector<Tile> tiles;
    std::ifstream file(filename);
    std::string line;
    int y = 0;
    while (std::getline(file, line)) {
        for (int x = 0; x < line.size(); ++x) {
            Tile tile;
            tile.x = x * TILE_SIZE;
            tile.y = y * TILE_SIZE;
            if (line[x] == '#') {
                tile.isWall = true;
                tiles.push_back(tile);
            } else if (line[x] == 'P') {
                // Set player's starting position.
                player.setX(x * TILE_SIZE);
                player.setY(y * TILE_SIZE);
                tile.isWall = false;
                tiles.push_back(tile);
            } else if (line[x] == 'D') {
                Duck duck;
                duck.setX(x * TILE_SIZE);
                duck.setY(y * TILE_SIZE);
                ducks.push_back(duck);
                tile.isWall = false;
                tiles.push_back(tile);
            } else if (line[x] == 'F') {
                Fox fox(x * TILE_SIZE, y * TILE_SIZE);
                foxes.push_back(fox);
                tile.isWall = false;
                tiles.push_back(tile);
            } else {
                tile.isWall = false;
                tiles.push_back(tile);
            }
        }
        ++y;
    }
    return tiles;
}