#include "advancedMode.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include "nlohmann/json.hpp"

#include "helpers/advHelper.cpp"

using namespace std;
using json = nlohmann::json;


void advancedMode(){
    printUsage();
    std::string inString;
    std::unordered_map<std::string, std::size_t> leftBlocks;
    std::unordered_map<std::string, std::size_t> rightBlocks;
    std::size_t numBlocksRight = 0;
    std::size_t numBlocksLeft = 0;
    json block;
    json generation = {
        {"blocks", json::array()},
        {"$schema", "https://raw.githubusercontent.com/JanDeDobbeleer/oh-my-posh/main/themes/schema.json"},
        {"version", 4},
        {"final_space", true}
    };
    // Command Input
    std::cout << "Input a command: ";
    while(std::getline(std::cin, inString)){
        std::istringstream iss(inString);
        std::string token;
        std::vector<std::string> splicedString;
        // Splice input string
        while (iss >> token) {
            splicedString.push_back(token);
        }
        // create left 
        if(splicedString[0] == "create" && splicedString.size() == 2){
            block = create(splicedString[1]);
            generation["blocks"].push_back(block);
            if(splicedString[1] == "right"){
                numBlocksRight++;
            }
            if(splicedString[1] == "left"){
                numBlocksLeft++;
            }
        } else if (splicedString[0] == "generate" && splicedString.size() == 1) {
            std::cout << "The file containing your new prompt has been generated.\n";
            return; // file is always being generated to show the output.
        } else if (splicedString[0] == "cancel" && splicedString.size() == 1) {
            std::cout << "Shutting down, the file containing your prompt will not be generated.\n";
            std::remove("adv-generated-theme.omp.json"); // If cancel then remove the generated file as they dont want anything
            return;
        }
        if(splicedString.size() == 4 && ((splicedString[0] == "left" && numBlocksLeft >= std::stoi(splicedString[1])) || (splicedString[0] == "right" && numBlocksRight >= std::stoi(splicedString[1])))){
            if(splicedString[2] == "add"){
                add(generation, findBlock(generation, splicedString[0], splicedString[1]), splicedString[3]);
                showPrompt(generation);
            } else if(splicedString[2] == "edit"){
                edit(generation, findBlock(generation, splicedString[0], splicedString[1]), splicedString[3]);
                showPrompt(generation);
            } else if(splicedString[2] == "remove"){
                remove(generation, findBlock(generation, splicedString[0], splicedString[1]), splicedString[3]);
                showPrompt(generation);
            } else if(splicedString[2] == "swap"){
                swap(generation, findBlock(generation, splicedString[0], splicedString[1]), splicedString[4], splicedString[5]);
                showPrompt(generation);
            }
        }
        std::cout << "\nInput a command: ";
    }
}

/*

Commands:

    create [right, left]

    [block_type {right, left, xxx}] add [git, ...]

    [block_type{right, left, xxx}] edit [git, ... , already existing segment]

    [block_type {right, left, xxx}] remove [git, ..., already existing segment]

    [block_type {right, left, xxx}] swap [segment] -> to swap segments positions

    cancel

    generate
*/