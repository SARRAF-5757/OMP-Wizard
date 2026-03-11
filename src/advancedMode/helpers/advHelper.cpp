#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include "nlohmann/json.hpp"
#include "segMenu.hpp"

using json = nlohmann::json;


unordered_set<std::string> segOptions = 
{
    "git",
    "os",
    "shell",
    "session",
    "time",
    "path",
    "battery",
    "executiontime",
    "root",
    "status",
    "sysinfo",
    "spotify",
};

std::vector<unordered_set<std::string>> blockSegs;

void printUsage() {
    std::cout << 
    "Usage Menu:\n"
                 "\033[4mcreate\033[0m [left, right]                           create a left or right block, for multiple refer to them in the order "
                 "created\n"
                 "                                               i.e, left0, left1, left2, left3\n"
                 "[left, right] index \033[4madd\033[0m [segment type]         add a segment to an existing left or right block\n"
                 "[left, right] index \033[4medit\033[0m [segment type]        edit an existing segment in an existing left or right block\n"
                 "[left, right] index \033[4mremove\033[0m [segment type]      remove an existing segment from an existing left or right block\n"
                 "[left, right] index \033[4mswap\033[0m [segment type]        swap two existing segments in an existing left or right block\n"
                 "\033[4mgenerate\033[0m                                       end the program and generate your completed prompt\n"
                 "\033[4mcancel\033[0m                                         cancel both the program and generation of your prompt\n"
                 "\033[4mhelp\033[0m                                           print this usage menu again\n\n";
}

/*
create [left, right] [true, false]       create a left or right block, with newline setting true or false
[left, right] add [segment type]         add a segment to an existing left or right block
[left, right] edit [segment type]        edit an existing segment in an existing left or right block
[left, right] remove [segment type]      remove an existing segment from an existing left or right block
[left, right] swap [segment type]        swap two existing segments in an existing left or right block
generate                                 end the program and generate your completed prompt
cancel                                   cancel both the program and generation of your prompt
help
*/
bool errorHandler(std::vector<std::string> command, json& currentGen, std::size_t& numLeftBlocks, std::size_t& numRightBlocks) {
    std::cout << "Error has been found\n";   // TODO: do this usage.
    std::unordered_set<std::string> segmentCommands = { "add", "edit", "remove", "swap" };
    std::unordered_set<std::string> functions = { "generate", "cancel", "help" };
    // CREATE COMMAND
    if(command[0] == "create")
    {
        if(command.size() != 2)
        {
            return true; // error
        }

        if(command[1] == "right")
        {
            numRightBlocks++;
            return false; // no error
        }
        else if(command[1] == "left")
        {
            numLeftBlocks++;
            return false; // no error 
        } 
        else 
        {
            return true; // error
        }
    }

    if(command[0] == "left" || command[0] == "right")
    {
        if(command.size() != 4){
            return true; // error
        }

        if((command[1] == "left" && stoi(command[1]) >= numLeftBlocks) ||
            (command[1] == "right" && stoi(command[1]) >= numRightBlocks))
        {
            return true; // error
        }
        
        if(segmentCommands.find(command[2]) == segmentCommands.end())
        {
            return true; // error
        }

        if(functions.find(command[3]) == functions.end())
        {
            return true; // error
        }
    }
}


json create(std::string blockType) {
    blockSegs.push_back({});
    json block = {
        {      "type",      "prompt" },
        { "alignment",     blockType },
        {   "newline",          true },
        {  "segments", json::array() }
    };
    std::cout << "The " << blockType << " block has been created\n";
    return block;
}
void addIsland(json& currentGen, std::size_t blockNum, std::string island) {
    // the block we just added was an island
    std::size_t finalIndex = currentGen["blocks"][blockNum]["segments"].size() - 1;
    if (island == "Yes") {
        if (finalIndex > 0 && currentGen["blocks"][blockNum]["segments"][finalIndex - 1]["background"] != "transparent") {
            auto& leading = currentGen["blocks"][blockNum]["segments"].back()["leading_diamond"];
            leading = " " + leading.get<std::string>();
            std::cout << leading << "\n";
        }
    } else {
        if (finalIndex > 0 && currentGen["blocks"][blockNum]["segments"][finalIndex - 1]["island"] == true
            && currentGen["blocks"][blockNum]["segments"][finalIndex - 1]["background"] != "transparent") {
            auto& leading = currentGen["blocks"][blockNum]["segments"].back()["leading_diamond"];
            leading = " " + leading.get<std::string>();
            std::cout << leading << "\n";
        }
    }
}

void add(json& currentGen, std::size_t blockNum, std::string segmentType) {
    bool prevIsland = true;
    if(blockSegs[blockNum].find(segmentType) == blockSegs[blockNum].end()){
        blockSegs[blockNum].insert(segmentType);
    } else {
        std::cout << "This segment already exists within this block, please try again!\n";
        return;
    }

    if(currentGen["blocks"][blockNum]["segments"].size() > 0) { prevIsland = currentGen["blocks"][blockNum]["segments"].back()["island"]; }
    auto [fg, bg, island, lead, trail, templ, opt] = menuDisplay(segmentType, prevIsland, -1);
    if (fg == "cancel") {
        std::cout << "Your operation has been canceled!\n";
        return;
    }
    json newSegment = {
        {             "type", segmentType },
        {            "style",   "diamond" },
        {       "background",          bg },
        {       "foreground",          fg },
        {  "leading_diamond",          "" },
        { "trailing_diamond",          "" }
    };

    if (lead == "Custom") {
        std::cout << "Input your custom leading diamond: ";
        std::cin >> lead;
        newSegment["leading_diamond"] = lead;
    } else if (lead != "None") {
        newSegment["leading_diamond"] = lead;
    }

    if (trail == "Custom") {
        std::cout << "Input your custom trailing diamond: ";
        std::cin >> trail;
        newSegment["trailing_diamond"] = trail;
    } else if (trail != "None") {
        newSegment["trailing_diamond"] = trail;
    }

    if (island == "Yes") {
        newSegment["island"] = true;
    } else {
        newSegment["island"] = false;
    }
    /*
    if(templ == "Custom"){
        std::string templa;
        std::cout << "Input your custom template: ";
        std::cin >> templa;
        newSegment["template"] = templa;
    }
    if(opt == "Custom"){
        std::string option;
        std::cout << "Input your custom options: ";
        std::cin >> option;
        newSegment["options"] = "TODO:";
    }
    */
    currentGen["blocks"][blockNum]["segments"].push_back(newSegment);
    addIsland(currentGen, blockNum, island);
    std::cout << "The " << segmentType << " segment has been added to your prompt!\n";
}

void editIsland(json& currentGen, std::size_t blockNum, std::size_t segmentNum, std::string island, bool prevIsland, bool currIsland) {
    if (island == "Yes") {
        if (prevIsland == false) {
            if (segmentNum > 0 && currentGen["blocks"][blockNum]["segments"][segmentNum - 1]["background"] != "transparent") {
                auto& leading = currentGen["blocks"][blockNum]["segments"][segmentNum]["leading_diamond"];
                leading = " " + leading.get<std::string>();
            }
            if (currentGen["blocks"][blockNum]["segments"].size() - 1 > segmentNum) {
                auto& leading2 = currentGen["blocks"][blockNum]["segments"][segmentNum + 1]["leading_diamond"];
                leading2 = " " + leading2.get<std::string>();
            }
        }
    } else {
        if (prevIsland == true) {
            if (currentGen["blocks"][blockNum]["segments"].size() - 1 >= segmentNum + 1) {
                auto& leading = currentGen["blocks"][blockNum]["segments"][segmentNum + 1]["leading_diamond"];
                leading = leading.get<std::string>().substr(1);
            }
        }
        if(currIsland == true){
            if(currentGen["blocks"][blockNum]["segments"].size() - 1 >= segmentNum + 1) {
                auto& leading = currentGen["blocks"][blockNum]["segments"][segmentNum + 1]["leading_diamond"];
                leading = leading.get<std::string>().substr(1);
            }
        }
    }
}

void edit(json& currentGen, std::size_t blockNum, std::string segmentType) {
    bool prevIsland = true;
    bool currIsland = false;
    std::size_t segmentInd = 0;
    std::size_t prevSegment = 0;
    for (std::size_t i = 0; i < currentGen["blocks"][blockNum]["segments"].size(); i++) {
        if (currentGen["blocks"][blockNum]["segments"][i]["type"] == segmentType) {
            if(i == 0){
                prevIsland = false;
            } else {
                prevIsland = currentGen["blocks"][blockNum]["segments"][i-1]["island"];
            }
            currIsland = currentGen["blocks"][blockNum]["segments"][i]["island"];
            segmentInd = i;
            break;
        }
    }
    if(segmentInd == 0){
        prevSegment = 0;
    } else {
        prevSegment = segmentInd - 1;
    }
    std::cout << prevIsland << "\n";
    auto [fg, bg, island, lead, trail, templ, opt] = menuDisplay(segmentType, prevIsland, prevSegment);
    if (fg == "cancel") {
        std::cout << "Your operation has been canceled!\n";
        return;
    }
    json newSegment = {
        {             "type", segmentType },
        {            "style",   "diamond" },
        {       "background",          bg },
        {       "foreground",          fg },
        {  "leading_diamond",          "" },
        { "trailing_diamond",          "" }
    };

    if (lead == "Custom") {
        std::cout << "Input your custom leading diamond: ";
        std::cin >> trail;
        newSegment["leading_diamond"] = lead;
    } else if (lead != "None") {
        newSegment["leading_diamond"] = lead;
    }

    if (trail == "Custom") {
        std::cout << "Input your custom trailing diamond: ";
        std::cin >> trail;
        newSegment["trailing_diamond"] = trail;
    } else if (trail != "None") {
        newSegment["trailing_diamond"] = trail;
    }

    if (island == "Yes") {
        newSegment["island"] = true;
    } else {
        newSegment["island"] = false;
    }

    currentGen["blocks"][blockNum]["segments"][segmentInd] = newSegment;
    editIsland(currentGen, blockNum, segmentInd, island, prevIsland, currIsland);
    std::cout << "The " << segmentType << " segment has been edited!\n";
}


void remove(json& currentGen, std::size_t blockNum, std::string segmentType) {
    for (std::size_t i = 0; i < currentGen["blocks"][blockNum]["segments"].size(); i++) {
        if (currentGen["blocks"][blockNum]["segments"][i]["type"] == segmentType) {
            currentGen["blocks"][blockNum]["segments"].erase(currentGen["blocks"][blockNum]["segments"].begin() + i);
            break;
        }
    }
    std::cout << "The " << segmentType << " segment has been removed from your prompt!\n";
}


void swap(json& currentGen, std::size_t blockNum, std::string segmentType, std::string segmentType2) {
    std::size_t seg1Pos, seg2Pos;
    for (std::size_t i = 0; i < currentGen["blocks"][blockNum]["segments"].size(); i++) {
        if (currentGen["blocks"][blockNum]["segments"][i]["type"] == segmentType) {
            seg1Pos = i;
        } else if (currentGen["blocks"][blockNum]["segments"][i]["type"] == segmentType2) {
            seg2Pos = i;
        }
    }
    json tempSeg = currentGen["blocks"][blockNum]["segments"][seg1Pos];
    currentGen["blocks"][blockNum]["segments"][seg1Pos] = currentGen["blocks"][blockNum]["segments"][seg2Pos];
    currentGen["blocks"][blockNum]["segments"][seg2Pos] = tempSeg;
}


void showPrompt(json& currentGen) {
    std::cout << "Here is your updated prompt!\n";
    ofstream o("adv-generated-theme.omp.json");
    o << currentGen.dump(4);
    o.close();
    std::system("oh-my-posh print preview --config adv-generated-theme.omp.json --force");
}


std::size_t findBlock(json& currentGen, std::string blockType, std::string blockNum) {
    std::size_t count = 0;
    for (std::size_t i = 0; i < currentGen["blocks"].size(); i++) {
        if (currentGen["blocks"][i]["alignment"] == blockType) {
            if (count == std::stoi(blockNum)) {
                count = i;
                break;
            } else {
                count++;
            }
        }
    }
    return count;
}

/*
std::vector<std::string> git_choices = {

};

    ~//git, execution time, os, path, session, shell, time
auto templateHandling(std::string segmentType){
    if(segmentType == "git"){

        string colorModeTitle = "How should the prompt be colorized?";
        auto tabColorMode = Radiobox({
            .entries = color_mode_choices,
            .selected = &config.color_mode,
        });

    } else if (segmentType == "execution time"){

    } else if (segmentType == "os"){

    } else if (segmentType == "path"){

    } else if (segmentType == "session"){

    } else if (segmentType == "shell"){

    } else if (segmentType == "time"){

    }
}
*/
