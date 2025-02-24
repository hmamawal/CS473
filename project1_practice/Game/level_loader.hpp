// level_loader.hpp
#ifndef LEVEL_LOADER_HPP
#define LEVEL_LOADER_HPP

#include <string>
#include <vector>
#include "../classes/Player.hpp"
#include "../classes/Duck.hpp"
#include "../classes/Fox.hpp"
#include "../classes/Tile.hpp"
#include "../utilities/constants.hpp"

std::vector<Tile> loadMap(const std::string& filename, Player& player,
                          std::vector<Duck>& ducks, std::vector<Fox>& foxes);

#endif
