#include <algorithm>
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

std::string stripIslandSpacingPrefix(const std::string& leading) {
    std::string result = leading;
    while (!result.empty() && result[0] == ' ') {
        result = result.substr(1);
    }
    return result;
}

bool segmentNeedsIslandSpacing(const json& segments, std::size_t index) {
    if (index == 0) {
        return false;
    }
    const auto& prev = segments[index - 1];
    const auto& curr = segments[index];
    if (prev.value("background", "") == "transparent") {
        return false;
    }
    return curr.value("island", false) || prev.value("island", false);
}

bool segmentNeedsLeadingDiamond(const json& segments, std::size_t index) {
    return index == 0
        || segments[index].value("island", false)
        || (index > 0 && segments[index - 1].value("island", false));
}

void applyIslandSpacingToSegment(json& segments, std::size_t index) {
    auto& seg = segments[index];
    std::string leading = stripIslandSpacingPrefix(seg.value("leading_diamond", ""));

    if (segmentNeedsIslandSpacing(segments, index)) {
        seg["leading_diamond"] = " " + leading;
    } else {
        seg["leading_diamond"] = leading;
    }
}

void cleanupUnneededLeadingDiamonds(json& segments) {
    for (std::size_t i = 0; i < segments.size(); i++) {
        if (!segmentNeedsLeadingDiamond(segments, i)) {
            segments[i]["leading_diamond"] = "";
        }
    }
}

void refreshBlockIslandSpacing(json& currentGen, std::size_t blockNum) {
    auto& segments = currentGen["blocks"][blockNum]["segments"];
    for (std::size_t i = 0; i < segments.size(); i++) {
        applyIslandSpacingToSegment(segments, i);
    }
}

void refreshBlockIslandSpacingAfterRemove(json& currentGen, std::size_t blockNum) {
    refreshBlockIslandSpacing(currentGen, blockNum);
    cleanupUnneededLeadingDiamonds(currentGen["blocks"][blockNum]["segments"]);
}

std::string formatSegmentOptions() {
    std::vector<std::string> segments(segOptions.begin(), segOptions.end());
    std::sort(segments.begin(), segments.end());

    std::ostringstream oss;
    for (std::size_t i = 0; i < segments.size(); i++) {
        if (i > 0) {
            oss << ", ";
        }
        oss << "\033[1m" << segments[i] << "\033[0m";
    }
    return oss.str();
}

void printUsage() {
    std::cout << 
    "Usage Menu:\n"
                 "\033[4mcreate\033[0m [left, right]                           create a left or right block, for multiple refer to them in the order "
                 "created\n"
                 "                                               i.e, left0, left1, left2, left3\n"
                 "[left, right] index \033[4madd\033[0m [segment type]         add a segment to an existing left or right block\n"
                 "                                               segment types: " << formatSegmentOptions() << "\n"
                 "[left, right] index \033[4medit\033[0m [segment type]        edit an existing segment in an existing left or right block\n"
                 "[left, right] index \033[4mremove\033[0m [segment type]      remove an existing segment from an existing left or right block\n"
                 "\033[4mgenerate\033[0m                                       end the program and generate your completed prompt\n"
                 "\033[4mcancel\033[0m                                         cancel both the program and generation of your prompt\n"
                 "\033[4mhelp\033[0m                                           print this usage menu again\n\n";
}

/*
create [left, right] [true, false]       create a left or right block, with newline setting true or false
[left, right] add [segment type]         add a segment to an existing left or right block
[left, right] edit [segment type]        edit an existing segment in an existing left or right block
[left, right] remove [segment type]      remove an existing segment from an existing left or right block
generate                                 end the program and generate your completed prompt
cancel                                   cancel both the program and generation of your prompt
help
*/

bool errorHandler(std::vector<std::string> command, json& currentGen, int& numRightBlocks, int& numLeftBlocks) {
    std::unordered_set<std::string> segmentCommands = { "add", "edit", "remove" };
    std::unordered_set<std::string> functions = { "generate", "cancel", "help", "preview" };
    // CREATE COMMAND
    if(command[0] == "create")
    {
        if(command.size() != 2)
        {
            return true; // error
        }

        if(command[1] == "right")
        {
            return false; // no error
        }
        else if(command[1] == "left")
        {
            return false; // no error 
        } 
        else 
        {
            return true; // error
        }
    }
    
    if(command[0] == "help" || command[0] == "preview" || command[0] == "generate"){
        if(command.size() != 1){
            return true; // error
        }
        return false; // no error
    }

    if(command[0] == "left" || command[0] == "right")
    {
        if(command.size() != 4){
            return true; // error
        }

        if((command[0] == "left" && stoi(command[1]) >= numLeftBlocks) ||
            (command[0] == "right" && stoi(command[1]) >= numRightBlocks)){
            return true; // error
        }
        
        if(segmentCommands.find(command[2]) == segmentCommands.end()){
            return true; // error
        }

        if(segOptions.find(command[3]) == segOptions.end()){
            return true; // error
        }
        return false;
    }

    return true;
}


void reorderBlocksAlternating(json& currentGen) {
    std::vector<json> leftBlocks;
    std::vector<json> rightBlocks;
    std::vector<unordered_set<std::string>> leftSegs;
    std::vector<unordered_set<std::string>> rightSegs;

    for (std::size_t i = 0; i < currentGen["blocks"].size(); i++) {
        if (currentGen["blocks"][i]["alignment"] == "left") {
            leftBlocks.push_back(currentGen["blocks"][i]);
            leftSegs.push_back(blockSegs[i]);
        } else {
            rightBlocks.push_back(currentGen["blocks"][i]);
            rightSegs.push_back(blockSegs[i]);
        }
    }

    currentGen["blocks"] = json::array();
    blockSegs.clear();

    std::size_t leftIndex = 0;
    std::size_t rightIndex = 0;
    bool takeLeft = true;

    while (leftIndex < leftBlocks.size() || rightIndex < rightBlocks.size()) {
        if (takeLeft && leftIndex < leftBlocks.size()) {
            currentGen["blocks"].push_back(leftBlocks[leftIndex]);
            blockSegs.push_back(leftSegs[leftIndex]);
            leftIndex++;
            takeLeft = false;
        } else if (!takeLeft && rightIndex < rightBlocks.size()) {
            currentGen["blocks"].push_back(rightBlocks[rightIndex]);
            blockSegs.push_back(rightSegs[rightIndex]);
            rightIndex++;
            takeLeft = true;
        } else if (leftIndex < leftBlocks.size()) {
            currentGen["blocks"].push_back(leftBlocks[leftIndex]);
            blockSegs.push_back(leftSegs[leftIndex]);
            leftIndex++;
        } else {
            currentGen["blocks"].push_back(rightBlocks[rightIndex]);
            blockSegs.push_back(rightSegs[rightIndex]);
            rightIndex++;
        }
    }

    std::size_t leftCount = 0;
    for (auto& block : currentGen["blocks"]) {
        if (block["alignment"] == "left") {
            block["newline"] = leftCount > 0;
            leftCount++;
        } else {
            block["newline"] = false;
        }
    }
}

json create(std::string blockType, size_t numBlocksLeft, size_t numBlocksRight) {
    blockSegs.push_back({});
    bool newline = true;
    if(blockType == "left" && numBlocksLeft == 0){
        newline = false;
    }
    if(blockType == "right" && numBlocksRight == 0){
        newline = false;
    }
    json block = {
        {      "type",      "prompt" },
        { "alignment",     blockType },
        {   "newline",       newline },
        {  "segments", json::array() }
    };

    std::cout << "The " << blockType << " block has been created\n";
    return block;
}

void add(json& currentGen, std::size_t blockNum, std::string segmentType) {
    bool prevIsland = false;
    if(blockSegs[blockNum].find(segmentType) == blockSegs[blockNum].end()){
        blockSegs[blockNum].insert(segmentType);
    } else {
        std::cout << "This segment already exists within this block, please try again!\n";
        return;
    }

    bool isFirstSegment = currentGen["blocks"][blockNum]["segments"].empty();
    if (!isFirstSegment) {
        prevIsland = currentGen["blocks"][blockNum]["segments"].back().value("island", false);
    }
    auto [fg, bg, island, lead, trail, templ, opt] = menuDisplay(segmentType, isFirstSegment, prevIsland);
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
    refreshBlockIslandSpacing(currentGen, blockNum);
    std::cout << "The " << segmentType << " segment has been added to your prompt!\n";
}

void edit(json& currentGen, std::size_t blockNum, std::string segmentType) {
    if (blockSegs[blockNum].find(segmentType) == blockSegs[blockNum].end()) {
        std::cout << "This segment does not exist within this block, please try again!\n";
        return;
    }

    bool prevIsland = false;
    bool currIsland = false;
    std::size_t segmentInd = 0;
    bool found = false;
    for (std::size_t i = 0; i < currentGen["blocks"][blockNum]["segments"].size(); i++) {
        if (currentGen["blocks"][blockNum]["segments"][i]["type"] == segmentType) {
            if (i > 0) {
                prevIsland = currentGen["blocks"][blockNum]["segments"][i - 1].value("island", false);
            }
            currIsland = currentGen["blocks"][blockNum]["segments"][i].value("island", false);
            segmentInd = i;
            found = true;
            break;
        }
    }
    if (!found) {
        std::cout << "This segment does not exist within this block, please try again!\n";
        return;
    }

    const bool isFirstSegment = segmentInd == 0;
    auto [fg, bg, island, lead, trail, templ, opt] = menuDisplayEdit(segmentType, isFirstSegment, prevIsland, currIsland);
    if (fg == "cancel") {
        std::cout << "Your operation has been canceled!\n";
        return;
    }

    const auto& existing = currentGen["blocks"][blockNum]["segments"][segmentInd];
    json newSegment = {
        {             "type", segmentType },
        {            "style",   "diamond" },
        {       "background",          bg },
        {       "foreground",          fg },
        {  "leading_diamond",          "" },
        { "trailing_diamond",          "" }
    };

    if (existing.contains("island")) {
        newSegment["island"] = existing["island"];
    }

    if (isFirstSegment || prevIsland || currIsland) {
        if (lead == "Custom") {
            std::cout << "Input your custom leading diamond: ";
            std::cin >> lead;
            newSegment["leading_diamond"] = lead;
        } else if (lead != "None") {
            newSegment["leading_diamond"] = lead;
        }
    }

    if (trail == "Custom") {
        std::cout << "Input your custom trailing diamond: ";
        std::cin >> trail;
        newSegment["trailing_diamond"] = trail;
    } else if (trail != "None") {
        newSegment["trailing_diamond"] = trail;
    }

    currentGen["blocks"][blockNum]["segments"][segmentInd] = newSegment;
    refreshBlockIslandSpacing(currentGen, blockNum);
    std::cout << "The " << segmentType << " segment has been edited!\n";
}

void remove(json& currentGen, std::size_t blockNum, std::string segmentType) {
    if (blockSegs[blockNum].find(segmentType) == blockSegs[blockNum].end()) {
        std::cout << "This segment does not exist within this block, please try again!\n";
        return;
    }

    for (std::size_t i = 0; i < currentGen["blocks"][blockNum]["segments"].size(); i++) {
        if (currentGen["blocks"][blockNum]["segments"][i]["type"] == segmentType) {
            currentGen["blocks"][blockNum]["segments"].erase(currentGen["blocks"][blockNum]["segments"].begin() + i);
            blockSegs[blockNum].erase(segmentType);
            refreshBlockIslandSpacingAfterRemove(currentGen, blockNum);
            std::cout << "The " << segmentType << " segment has been removed from your prompt!\n";
            return;
        }
    }

    std::cout << "This segment does not exist within this block, please try again!\n";
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
