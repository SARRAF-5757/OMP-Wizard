#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include "nlohmann/json.hpp"

#include "segMenu.hpp"

using namespace std;
using json = nlohmann::json;


void printUsage(){
    std::cout << 
    "Usage Menu:\n"
    "\033[4mcreate\033[0m [left, right]                           create a left or right block, for multiple refer to them in the order created\n"
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
//TODO: redo this ngl its poorly written
bool errorHandler(std::vector<std::string> command, json& currentGen){
    std::cout << "Error has been found\n"; //TODO: do this usage.
    std::unordered_set<std::string> segmentCommands = {"add", "edit", "remove", "swap"};
    std::unordered_set<std::string> functions = {"generate", "cancel", "help"};
    std::unordered_set<std::string> blocks;
    std::unordered_set<std::string> segments;
    for(std::size_t i = 0; i < currentGen["blocks"].size(); i++){
        blocks.insert(currentGen["blocks"][i]["alignment"]);
    }
    if(command.size() == 0){
        std::cout << "No input, try again with one of the commands listed below\n";
        printUsage();
        return true;
    } else if(command.size() == 1){
        if(functions.find(command[0]) == functions.end()){
            std::cout << "Invalid command, please try again\n";
            return true;
        }
        return false;
    } else if(command.size() == 4){
        return false;
    } else {
        std::cout << "Your command is of an incorrect size, please try again\n";
        return true;
    }
}


json create(std::string blockType){
    json block = {
        { "type", "prompt"},
        { "alignment", blockType},
        { "newline", true},
        { "segments", json::array()}
    };
    std::cout << "The " << blockType << " block has been created\n";
    return block;
}
void addIsland(json &currentGen, std::size_t blockNum, std::string island){
    // the block we just added was an island
    std::size_t finalIndex = currentGen["blocks"][blockNum]["segments"].size()-1;
    if(island == "Yes"){
        if(finalIndex > 0 && currentGen["blocks"][blockNum]["segments"][finalIndex-1]["background"] != "transparent"){
            auto& leading = currentGen["blocks"][blockNum]["segments"].back()["leading_diamond"];
            leading = " " + leading.get<std::string>();
        }
    } else {
        if(finalIndex > 0 && currentGen["blocks"][blockNum]["segments"][finalIndex-1]["island"] == true 
            && currentGen["blocks"][blockNum]["segments"][finalIndex-1]["background"] != "transparent"){
            auto& leading = currentGen["blocks"][blockNum]["segments"].back()["leading_diamond"];
            leading = " " + leading.get<std::string>();
        }
    }
}

void add(json &currentGen, std::size_t blockNum, std::string segmentType){
    auto [fg, bg, island, lead ,trail, templ, opt] = menuDisplay(segmentType);
    if(fg == "cancel"){
        std::cout << "Your operation has been canceled!\n";
        return;
    }
    json newSegment = {
        {       "type",        segmentType },
        {      "style",       "diamond" },
        { "background",       bg },
        { "foreground", fg },
        {"leading_diamond", ""},
        {"trailing_diamond", ""}
    };

    if(lead == "Custom"){
        std::cout << "Input your custom leading diamond: ";
        std::cin >> lead;
        newSegment["leading_diamond"] = lead;
    } else if(lead != "None"){
        newSegment["leading_diamond"] = lead;
    }

    if(trail == "Custom"){
        std::cout << "Input your custom trailing diamond: ";
        std::cin >> trail;
        newSegment["trailing_diamond"] = trail;
    } else if (trail != "None"){
        newSegment["trailing_diamond"] = trail;
    }

    if(island == "Yes"){
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

void editIsland(json &currentGen, std::size_t blockNum, std::size_t segmentNum, std::string island, bool prevIsland){
    if(island == "Yes"){
        if(prevIsland == false){
            if(segmentNum > 0 && currentGen["blocks"][blockNum]["segments"][segmentNum-1]["background"] != "transparent"){
                auto& leading = currentGen["blocks"][blockNum]["segments"][segmentNum]["leading_diamond"];
                leading = " " + leading.get<std::string>();
            }
            if(currentGen["blocks"][blockNum]["segments"].size()-1 > segmentNum){
                auto& leading2 = currentGen["blocks"][blockNum]["segments"][segmentNum+1]["leading_diamond"];
                leading2 = " " + leading2.get<std::string>();
            }
        }
    } else {
        if(prevIsland == true){
            if(currentGen["blocks"][blockNum]["segments"].size()-1 > segmentNum){
                auto& leading = currentGen["blocks"][blockNum]["segments"][segmentNum+1]["leading_diamond"];
                leading = leading.get<std::string>().substr(1);
            }
        }
    }
}

void edit(json &currentGen, std::size_t blockNum, std::string segmentType){
    auto [fg, bg, island, lead ,trail, templ, opt] = menuDisplay(segmentType);
    std::size_t segmentInd = 0;
    bool prevIsland = false;
    if(fg == "cancel"){
        std::cout << "Your operation has been canceled!\n";
        return;
    }
    json newSegment = {
        {       "type",        segmentType },
        {      "style",       "diamond" },
        { "background",       bg },
        { "foreground", fg },
        {"leading_diamond", ""},
        {"trailing_diamond", ""}
    };

    if(lead == "Custom"){
        std::cout << "Input your custom leading diamond: ";
        std::cin >> trail;
        newSegment["leading_diamond"] = lead;
    } else if(lead != "None"){
        newSegment["leading_diamond"] = lead;
    }

    if(trail == "Custom"){
        std::cout << "Input your custom trailing diamond: ";
        std::cin >> trail;
        newSegment["trailing_diamond"] = trail;
    } else if (trail != "None"){
        newSegment["trailing_diamond"] = trail;
    }

    if(island == "Yes"){
        newSegment["island"] = true;
    } else {
        newSegment["island"] = false;
    }

    for(std::size_t i = 0; i < currentGen["blocks"][blockNum]["segments"].size(); i++){
        if(currentGen["blocks"][blockNum]["segments"][i]["type"] == segmentType){
            currentGen["blocks"][blockNum]["segments"][i] = newSegment;
            prevIsland = currentGen["blocks"][blockNum]["segments"][i]["island"];
            segmentInd = i;
            break;
        }
    }
    editIsland(currentGen, blockNum, segmentInd, island, prevIsland);
    std::cout << "The " << segmentType << " segment has been edited!\n";
}


void remove(json &currentGen, std::size_t blockNum, std::string segmentType){
    for(std::size_t i = 0; i < currentGen["blocks"][blockNum]["segments"].size(); i++){
        if(currentGen["blocks"][blockNum]["segments"][i]["type"] == segmentType){
            currentGen["blocks"][blockNum]["segments"].erase(currentGen["blocks"][blockNum]["segments"].begin() + i);
            break;
        }
    }
    std::cout << "The " << segmentType << " segment has been removed from your prompt!\n";
}


void swap(json &currentGen, std::size_t blockNum, std::string segmentType, std::string segmentType2){
    std::size_t seg1Pos, seg2Pos;
    for(std::size_t i = 0; i < currentGen["blocks"][blockNum]["segments"].size(); i++){
        if(currentGen["blocks"][blockNum]["segments"][i]["type"] == segmentType){
            seg1Pos = i;
        } else if (currentGen["blocks"][blockNum]["segments"][i]["type"] == segmentType2){
            seg2Pos = i;
        }
    }
    json tempSeg = currentGen["blocks"][blockNum]["segments"][seg1Pos];
    currentGen["blocks"][blockNum]["segments"][seg1Pos] = currentGen["blocks"][blockNum]["segments"][seg2Pos];
    currentGen["blocks"][blockNum]["segments"][seg2Pos] = tempSeg;
}


void showPrompt(json &currentGen){
    std::cout << "Here is your updated prompt!\n";
    ofstream o("adv-generated-theme.omp.json");
    o << currentGen.dump(4);
    o.close();
    std::system("oh-my-posh print preview --config adv-generated-theme.omp.json --force");
}


std::size_t findBlock(json &currentGen, std::string blockType, std::string blockNum){
    std::size_t count = 0;
    for(std::size_t i = 0; i < currentGen["blocks"].size(); i++){
        if(currentGen["blocks"][i]["alignment"] == blockType){
            if(count == std::stoi(blockNum)){
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