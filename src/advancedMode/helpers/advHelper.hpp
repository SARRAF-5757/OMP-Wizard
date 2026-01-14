#include <iostream>

void printUsage();

bool commandEC(std::vector<std::string> command, json& currentGen);

json create(std::string blockType, std::string newlineSetting);

void addIsland(json &currentGen, std::size_t blockNum, std::string island);

void add(json &currentGen, std::size_t blockNum, std::string segmentType);

void editIsland(json &currentGen, std::size_t blockNum, std::size_t segmentNum, std::string island, bool prevIsland);

void edit(json &currentGen, std::size_t blockNum, std::string segmentType);

void remove(json &currentGen, std::size_t blockNum, std::string segmentType);

void swap(json &currentGen, std::size_t blockNum, std::string segmentType1, std::string segmentType2);

void swapIsland(json &currentGen, std::size_t blockNum, std::string segmentType, std::string trail, std::string lead, std::string island);

void findBlock(json &currentGen);

void showPrompt(json &currentGen, std::size_t blockNum);

auto templateHandling(std::string segmentType);