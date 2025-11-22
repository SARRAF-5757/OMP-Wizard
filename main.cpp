#include <fstream>

#include <ftxui/component/component_base.hpp>

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "nlohmann/json.hpp"

using namespace std;
using namespace ftxui;
using json = nlohmann::json;

//^ Struct to hold different user choices
struct ConfigState {
    // block choices
    bool show_user = false;
    bool show_path = false;
    bool show_git = false;
    // diamond choices
    int dmnd_leading = 0;
    int dmnd_connecting = 0;
    int dmnd_trailing = 0;
};

// Dictionary of diamonds for human-readable usage throughout the code
map<string, string> symbols = {
    {   "lCirc", "\ue0b6" },
    {   "rCirc", "\ue0b4" },
    {    "lTri", "\ue0b2" },
    {    "rTri", "\ue0b0" },
    { "llSlant", "\ue0ba" },
    { "rlSlant", "\ue0bc" },
    { "luSlant", "\ue0be" },
    { "ruSlant", "\ue0b8" },
    {   "lFire", "\ue0c2" },
    {   "rFire", "\ue0c0" },
    {  "lShock", "\ue0ca" },
    {  "rShock", "\ue0c8" },
    {  "lbFade", "\ue0c7" },
    {  "rbFade", "\ue0c6" },
    {  "lsFade", "\ue0c5" },
    {  "rsFade", "\ue0c4" },
};


//^ List of leading diamond choices (radiobox entries)
vector<string> leading_diamonds = {
    "\ue0b6",   // left half-circle
    "\ue0b2",   // left triangle
    "\ue0ba",   // left lower slant
    "\ue0be",   // left upper slant
    "\ue0c2",   // left fire
    "\ue0ca",   // left shockwave
    "\ue0c7",   // left big square fade
    "\ue0c5",   // left small square fade
};

//^ List of trailing diamond choices (radiobox entries)
vector<string> trailing_diamonds = {
    "\ue0b4",   // right half-circle
    "\ue0b0",   // right triangle
    "\ue0bc",   // right upper slant
    "\ue0b8",   // right lower slant
    "\ue0c0",   // right fire
    "\ue0c8",   // right shockwave
    "\ue0c6",   // right big square fade
    "\ue0c4",   // right small square fade
};

//!--------------------------------------Generate JSON-------------------------------------!//
void GenerateJSON(const ConfigState& config) {
    // initialize vectors of segment properties and populate them based on selected segments
    vector<string> types;
    vector<string> templates;
    vector<string> colors;
    vector<json> segmentsJSON;

    if (config.show_user) {
        types.emplace_back("session");
        templates.emplace_back(" {{ .UserName }} ");
        colors.emplace_back("#BDA5FE");
    }
    if (config.show_path) {
        types.emplace_back("path");
        templates.emplace_back(" \ue5ff {{ .Path }} ");
        colors.emplace_back("#539aff");
    }
    if (config.show_git) {
        types.emplace_back("git");
        templates.emplace_back(
          " {{ .UpstreamIcon }}{{ .HEAD }}{{if .BranchStatus }} {{ .BranchStatus }}{{ end }}{{ if .Working.Changed }} "
          "\uf044 {{ .Working.String }}{{ end }}{{ if and (.Working.Changed) (.Staging.Changed) }} |{{ end }}{{ if "
          ".Staging.Changed }} \uf046 {{ .Staging.String }}{{ end }}{{ if gt .StashCount 0 }} \ueb4b {{ .StashCount "
          "}}{{ end }} ");
        colors.emplace_back("#fffd9c");
    }

    //^ First, just create all the segments with their basic properties
    for (size_t i = 0; i < types.size(); ++i) {
        json individualSegment = {
            {       "type",     types[i] },
            {   "template", templates[i] },
            {      "style",    "diamond" },
            { "background",    colors[i] },
            { "foreground",    "#000000" }
        };
        segmentsJSON.push_back(individualSegment);
    }

    //^ Now add diamonds based on position
    if (types.size() == 1) {
        segmentsJSON[0]["leading_diamond"] = leading_diamonds[config.dmnd_leading];
        segmentsJSON[0]["trailing_diamond"] = trailing_diamonds[config.dmnd_trailing];
    } else {
        for (size_t i = 0; i < types.size(); i++) {
            if (i == 0) {
                segmentsJSON[i]["leading_diamond"] = leading_diamonds[config.dmnd_leading];
                segmentsJSON[i]["trailing_diamond"] = trailing_diamonds[config.dmnd_connecting];
            } else if (i == types.size() - 1) {
                segmentsJSON.back()["trailing_diamond"] = trailing_diamonds[config.dmnd_trailing];
            } else {
                segmentsJSON[i]["trailing_diamond"] = trailing_diamonds[config.dmnd_connecting];
            }
        }
    }

    //^ Special Properties
    for (json& segment : segmentsJSON) {
        if (segment["type"] == "path") {
            segment["properties"] = {
                { "style", "folder" }
            };
        }
        if (segment["type"] == "git") {
            segment["properties"] = {
                {         "branch_icon", " \ue725 " },
                {        "fetch_status",       true },
                { "fetch_upstream_icon",       true }
            };
        }
    }

    //* Form our JSON object, starting with just one block that contains an array.
    json j = {
        { "blocks", json::array() }
    };

    // add hardcoded default stuff
    j["$schema"] = "https://raw.githubusercontent.com/JanDeDobbeleer/oh-my-posh/main/themes/schema.json";
    j["version"] = 1;
    j["final_space"] = true;
    j["blocks"].push_back({
      {      "type",     "prompt" },
      { "alignment",       "left" },
      {  "segments", segmentsJSON },
    });

    // output JSON into a file with a 4-indent style (basic JSON formatting)
    ofstream o("temp.omp.json");
    o << j.dump(4);
    o.close();
}

//!----------------------------------------MAIN----------------------------------------!//
int main() {
    // Initialize Stuff
    auto screen = ScreenInteractive::Fullscreen();
    int tabSelected = 0;
    ConfigState config;

    //* Content inside each tab
    auto tabChooseBlocks = Container::Vertical({
      Checkbox("Show User", &config.show_user),
      Checkbox("Directory Path", &config.show_path),
      Checkbox("Git Status", &config.show_git),
      // TODO: add more block choices
    });

    auto tabDmndLeading = Radiobox({
      .entries = leading_diamonds,
      .selected = &config.dmnd_leading,
    });

    string dmndConnectTitle = "Choose Connecting Diamond to Show in your Prompt";
    auto tabDmndConnecting = Radiobox({
      .entries = trailing_diamonds,
      .selected = &config.dmnd_connecting,
    });

    string dmndTrailTitle = "Choose Trailing Diamond to Show in your Prompt";
    auto tabDmndTrailing = Radiobox({
      .entries = trailing_diamonds,
      .selected = &config.dmnd_trailing,
    });


    //^ Vectors to switch contents among tabs (preloaded with first two screens)
    // Vector of components representing each tab
    vector<Component> showTabs = { tabChooseBlocks, tabDmndLeading };
    // Titles shown on each screens
    vector<string> tabMessage
      = { "Choose Components to Show in your Prompt", "Choose Leading Diamond to Show in your Prompt" };


    // Container of the differing tab blocks
    auto tabContainer = Container::Tab(
      {
        showTabs,
      },
      &tabSelected);

    // Container representing the viewed tab
    auto container = Container::Vertical({
      tabContainer,
    });

    // Component to render
    auto toRender = Renderer(container, [&] {
        // Display components vertically
        return vbox({ text(tabMessage[tabSelected]) | hcenter, text(" "), tabContainer->Render() | hcenter, text(" "),
                      text(" "),
                      vbox({ text("[q] = Quit the wizard without generating config file"),
                             text("[n] = Confirm selections & go to the next screen"),
                             text("[e] = End the wizard and generate config file"),
                             text("      (will use defaults if all options are not specified)") })
                        | hcenter })
             | vcenter;
    });

    // To catch 'q' keypress to exit wizard
    auto component = CatchEvent(toRender, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Character('n')) {
            if (tabSelected < showTabs.size() - 1) {
                // if only one block is selected, only offer leading and trailing diamond
                if (tabSelected == 0 && (config.show_path ^ config.show_git)) {
                    showTabs.push_back(tabDmndTrailing);
                    tabMessage.push_back(dmndTrailTitle);
                } else if (tabSelected == 0) {
                    // if multiple blocks are choosen, show all diamond choices
                    showTabs.push_back(tabDmndConnecting);
                    tabMessage.push_back(dmndConnectTitle);

                    showTabs.push_back(tabDmndTrailing);
                    tabMessage.push_back(dmndTrailTitle);
                }

                // stop advancing if no blocks are chosen or if at the last page
                if ((config.show_path || config.show_git) && (tabSelected < showTabs.size() - 1)) {
                    tabSelected = tabSelected + 1;
                    tabContainer = Container::Tab(
                      {
                        showTabs,
                      },
                      &tabSelected);
                    container->DetachAllChildren();
                    container->Add(tabContainer);
                }
            }
        }
        if (event == Event::Character('e')) {
            GenerateJSON(config);
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    // Loop and render component on the screen
    screen.Loop(component);

    return 0;
}
