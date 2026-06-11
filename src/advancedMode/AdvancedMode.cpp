#include "advancedMode.hpp"

#include "helpers/advHelper.cpp"
#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

using json = nlohmann::json;


void advancedMode() 
{
    printUsage();
    int numBlocksRight{ 0 };
    int numBlocksLeft{ 0 };
    json generation = 
    {
        {      "blocks",                                                                         json::array() },
        {     "$schema", "https://raw.githubusercontent.com/JanDeDobbeleer/oh-my-posh/main/themes/schema.json" },
        {     "version",                                                                                     4 },
        { "final_space",                                                                                  true }
    };

    // Command Input
    std::string inString;
    std::cout << "Input a command: ";

    while (std::getline(std::cin, inString)){
        // Splice input string
        std::istringstream iss(inString);
        std::string token;
        std::vector<std::string> splicedString;
        while (iss >> token){
            splicedString.push_back(token);
        }

        if(splicedString[0] == "help"){
            if(errorHandler(splicedString, generation, numBlocksRight, numBlocksLeft)){
                std::cout << "Error in your command, try again\n";
            } else {
                printUsage();
            }
        }

        if(splicedString[0] == "preview"){
            if(errorHandler(splicedString, generation, numBlocksRight, numBlocksLeft)){
                std::cout << "Error in your command, try again\n";
            } else {
                std::system("oh-my-posh print preview --config adv-generated-theme.omp.json --force");
            }
        }

        if(splicedString[0] == "create"){
            if(errorHandler(splicedString, generation, numBlocksRight, numBlocksLeft)){
                std::cout << "Error in your command, try again\n";
            }
            else {
                generation["blocks"].push_back(create(splicedString[1], numBlocksLeft, numBlocksRight));
                if (splicedString[1] == "right"){
                    numBlocksRight++;
                }
                if (splicedString[1] == "left"){
                    numBlocksLeft++;
                }
                reorderBlocksAlternating(generation);
            }
        } else if (splicedString[0] == "generate" && splicedString.size() == 1){
            std::cout << "The file containing your new prompt has been generated.\n";
            return;   // file is always being generated to show the output.
        } else if (splicedString[0] == "cancel" && splicedString.size() == 1){
            std::cout << "Shutting down, the file containing your prompt will not be generated.\n";
            std::remove("adv-generated-theme.omp.json");   // If cancel then remove the generated file as they dont want anything
            return;
        } else if (splicedString[0] == "left" || splicedString[0] == "right"){
            if(errorHandler(splicedString, generation, numBlocksRight, numBlocksLeft)){
                std::cout << "Error in your command, try again\n";
            }
            else if (splicedString[2] == "add"){
                add(generation, findBlock(generation, splicedString[0], splicedString[1]), splicedString[3]);
                showPrompt(generation);
            } 
            else if (splicedString[2] == "edit"){
                edit(generation, findBlock(generation, splicedString[0], splicedString[1]), splicedString[3]);
                showPrompt(generation);
            } 
            else if (splicedString[2] == "remove"){
                remove(generation, findBlock(generation, splicedString[0], splicedString[1]), splicedString[3]);
                showPrompt(generation);
            }
        } else {
            std::cout << "Error in your command, try again\n";
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
