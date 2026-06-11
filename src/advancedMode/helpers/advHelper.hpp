#include <iostream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

void printUsage();

bool commandEC(std::vector<std::string> command, json& currentGen);

json create(std::string blockType, std::string newlineSetting);

void add(json &currentGen, std::size_t blockNum, std::string segmentType);

void edit(json &currentGen, std::size_t blockNum, std::string segmentType);

void remove(json &currentGen, std::size_t blockNum, std::string segmentType);

void findBlock(json &currentGen);

void showPrompt(json &currentGen, std::size_t blockNum);

auto templateHandling(std::string segmentType);