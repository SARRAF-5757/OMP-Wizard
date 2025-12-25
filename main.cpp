#include <fstream>

#include <ftxui/component/component_base.hpp>

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "nlohmann/json.hpp"

using namespace std;
using namespace ftxui;
using json = nlohmann::json;

// rgb color struct (defaults to light purple)
struct rgb {
    int red = 156;
    int green = 115;
    int blue = 254;
};

//* Struct to hold different user choices
struct ConfigState {
    // block choices
    bool show_user = false;
    bool show_path = false;
    bool show_git = false;
    // block colors
    string color_fg = "transparent";
    string color_bg = "#000000";
    string color_user = "#FFFFFF";
    string color_path = "#FFFFFF";
    string color_git = "#FFFFFF";
    // diamond choices
    int dmnd_leading = 0;
    int dmnd_connecting = 0;
    int dmnd_trailing = 0;
    // misc options
    int tr_prompt = 0;
    int color_mode = 0;
    int title_mode = 0;

    // colors
    rgb fg_color = { 0, 0, 0 };
    rgb bg_color = { 18, 19, 20 };
    rgb user_color = { 171, 118, 217 };
    rgb path_color = { 130, 170, 255 };
    rgb git_color = { 255, 223, 109 };
};

//* Dictionary of diamonds for human-readable usage throughout the code
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


//* List of leading diamond choices (radiobox entries)
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

//* List of trailing diamond choices (radiobox entries)
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

//* For miscellaneous options
vector<string> boolean_choice = { "Yes", "No" };
vector<string> color_mode_choices
  = { "Colored Background", "Colored Text (Transparent Background)", "Colored Text (Monochrome Background)" };
vector<string> title_choices
  = { "Default", "Current folder", "Parent folder/current folder", "Full Path", "'Shell' in 'Full Path'" };


//!--------------------------------------Hlper Functions-------------------------------------!//
//@ Helper to convert rgba values to HEX codes
string RGBtoHex(int r, int g, int b) {
    stringstream ss;
    ss << "#" << hex << setfill('0') << setw(2) << r << setw(2) << g << setw(2) << b;
    return ss.str();
}

//@ To create tab components for color selector tabs
Component colorPicker(rgb* rgbValues, string& hex) {
    auto slider_r = Slider("Red   :", &rgbValues->red, 0, 255, 1);
    auto slider_g = Slider("Green :", &rgbValues->green, 0, 255, 1);
    auto slider_b = Slider("Blue  :", &rgbValues->blue, 0, 255, 1);

    auto layout = Container::Vertical({ slider_r, slider_g, slider_b });

    return Renderer(layout, [=, &hex] {
        auto preview = text("   COLOR   ") | bgcolor(Color::RGB(rgbValues->red, rgbValues->green, rgbValues->blue))
                     | color(Color::Black);
        hex = RGBtoHex(rgbValues->red, rgbValues->green, rgbValues->blue);

        // the size() options are necessary to make it wide enough.
        // or else it shrinks it too much (possibly cuz of hcenter in the main renderer)
        return hbox({ preview, separator(),
                      vbox({ slider_r->Render(), slider_g->Render(), slider_b->Render(), separator(),
                             text("Hex: " + hex) })
                        | size(WIDTH, GREATER_THAN, 30) })
             | border | size(WIDTH, GREATER_THAN, 30);
    });
}


//!--------------------------------------Generate JSON-------------------------------------!//
void GenerateJSON(const ConfigState& config) {
    //? Initialize vectors of segment properties and populate them based on selected segments
    vector<string> types;
    vector<string> templates;
    vector<string> colors;
    vector<json> segmentsJSON;

    if (config.show_user) {
        types.emplace_back("session");
        templates.emplace_back(" {{ .UserName }} ");
        colors.push_back(config.color_user);
    }
    if (config.show_path) {
        types.emplace_back("path");
        templates.emplace_back(" \ue5ff {{ .Path }} ");
        colors.push_back(config.color_path);
    }
    if (config.show_git) {
        types.emplace_back("git");
        templates.emplace_back(
          " {{ .UpstreamIcon }}{{ .HEAD }}{{if .BranchStatus }} {{ .BranchStatus }}{{ end }}{{ if .Working.Changed }} "
          "\uf044 {{ .Working.String }}{{ end }}{{ if and (.Working.Changed) (.Staging.Changed) }} |{{ end }}{{ if "
          ".Staging.Changed }} \uf046 {{ .Staging.String }}{{ end }}{{ if gt .StashCount 0 }} \ueb4b {{ .StashCount "
          "}}{{ end }} ");
        colors.push_back(config.color_git);
    }

    //? First, just create all the segments with their basic properties
    // swap where color goes based on color mode
    if (config.color_mode == 0) {
        for (size_t i = 0; i < types.size(); ++i) {
            json individualSegment = {
                {       "type",        types[i] },
                {   "template",    templates[i] },
                {      "style",       "diamond" },
                { "background",       colors[i] },
                { "foreground", config.color_fg }
            };
            segmentsJSON.push_back(individualSegment);
        }
    } else if (config.color_mode == 1) {
        for (size_t i = 0; i < types.size(); ++i) {
            json individualSegment = {
                {       "type",      types[i] },
                {   "template",  templates[i] },
                {      "style",     "diamond" },
                { "background", "transparent" },
                { "foreground",     colors[i] }
            };
            segmentsJSON.push_back(individualSegment);
        }
    } else {
        for (size_t i = 0; i < types.size(); ++i) {
            json individualSegment = {
                {       "type",        types[i] },
                {   "template",    templates[i] },
                {      "style",       "diamond" },
                { "background", config.color_bg },
                { "foreground",       colors[i] }
            };
            segmentsJSON.push_back(individualSegment);
        }
    }

    //? Now add diamonds based on position
    if (types.size() == 1) {
        // offset fix for some leading diamonds
        if (config.dmnd_leading > 3) {
            segmentsJSON[0]["leading_diamond"] = leading_diamonds[config.dmnd_leading] + " ";
        } else {
            segmentsJSON[0]["leading_diamond"] = leading_diamonds[config.dmnd_leading];
        }
        segmentsJSON[0]["trailing_diamond"] = trailing_diamonds[config.dmnd_trailing];
    } else {
        for (size_t i = 0; i < types.size(); i++) {
            if (i == 0) {
                if (config.dmnd_leading > 3) {
                    segmentsJSON[0]["leading_diamond"] = leading_diamonds[config.dmnd_leading] + " ";
                } else {
                    segmentsJSON[0]["leading_diamond"] = leading_diamonds[config.dmnd_leading];
                }
                segmentsJSON[i]["trailing_diamond"] = trailing_diamonds[config.dmnd_connecting];
            } else if (i == types.size() - 1) {
                segmentsJSON.back()["trailing_diamond"] = trailing_diamonds[config.dmnd_trailing];
            } else {
                segmentsJSON[i]["trailing_diamond"] = trailing_diamonds[config.dmnd_connecting];
            }
        }
    }

    //? Special Properties
    for (json& segment : segmentsJSON) {
        if (segment["type"] == "path") {
            segment["options"] = {
                { "style", "folder" }
            };
        }
        if (segment["type"] == "git") {
            segment["options"] = {
                {         "branch_icon", " \ue725 " },
                {        "fetch_status",       true },
                { "fetch_upstream_icon",       true }
            };
        }
    }

    //? Form our JSON object, starting with just one block that contains an array.
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

    //? add optional miscellaneous settings
    if (!config.tr_prompt) {
        j["transient_prompt"] = {
            {   "template",     "\ue285 " },
            { "foreground",     "#AB76D9" },
            { "background", "transparent" }
        };
    }

    if (config.title_mode == 1) {
        j["console_title_template"] = "{{ .Folder }}";
    } else if (config.title_mode == 2) {
        j["console_title_template"] = "{{ base (dir .PWD)}}/{{ .Folder }}";
    } else if (config.title_mode == 3) {
        j["console_title_template"] = "{{ .PWD }}";
    } else if (config.title_mode == 4) {
        j["console_title_template"] = "{{ .Shell }} in {{ .PWD }}";
    }

    // output JSON into a file with a 4-indent style (basic JSON formatting)
    ofstream o("generated-theme.omp.json");
    o << j.dump(4);
    o.close();
}

//!----------------------------------------MAIN----------------------------------------!//
int main() {
    // Initialize Stuff
    auto screen = ScreenInteractive::Fullscreen();
    int tabSelected = 0;
    ConfigState config;
    bool tabsAdded = false;

    //* Define content inside each tab
    string blocksTitle = "Choose Components to Show in your Prompt";
    auto tabChooseBlocks = Container::Vertical({
      Checkbox("Show User", &config.show_user),
      Checkbox("Directory Path", &config.show_path),
      Checkbox("Git Status", &config.show_git),
      // TODO: add more block choices
    });

    string colorModeTitle = "How should the prompt be colorized?";
    auto tabColorMode = Radiobox({
      .entries = color_mode_choices,
      .selected = &config.color_mode,
    });

    string trPromptTitle
      = "Transient Prompt: Do you want to reduce clutter by shrinking your prompt after you hit Enter?";
    auto tabTrPrompt = Radiobox({
      .entries = boolean_choice,
      .selected = &config.tr_prompt,
    });

    string titleTitle = "What should the terminal tab titles look like?";
    auto tabTitle = Radiobox({
      .entries = title_choices,
      .selected = &config.title_mode,
    });

    // # Block slection based pages
    string dmndLeadTitle = "Choose Leading Diamond to Show in your Prompt";
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

    string fgTitle = "Pick a text color";
    auto tabFg = colorPicker(&config.fg_color, config.color_fg);

    string bgTitle = "Pick a background color";
    auto tabBg = colorPicker(&config.bg_color, config.color_bg);

    string userColorTitle = "Pick a color for your user block";
    auto tabUserColor = colorPicker(&config.user_color, config.color_user);

    string dirColorTitle = "Pick a color for your directory block";
    auto tabDirColor = colorPicker(&config.path_color, config.color_path);

    string gitColorTitle = "Pick a color for your git block";
    auto tabGitColor = colorPicker(&config.git_color, config.color_git);


    //* Vectors to switch contents among tabs (preloaded with first two screens)
    vector<Component> showTabs = { tabChooseBlocks, tabColorMode };   // Vector of components representing each tab
    vector<string> tabMessage = { blocksTitle, colorModeTitle };      // Titles shown on each screens


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

    //* Catch keypresses
    // To catch 'q' keypress to exit wizard
    auto component = CatchEvent(toRender, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }

        if (event == Event::Character('n')) {
            // don't advance if no block is selected
            if (tabSelected == 0) {
                // don't advance if no block is selected
                if (!(config.show_user || config.show_path || config.show_git)) {
                    return true;
                }
            }

            // only add the rest of the tabs once, while on block selection (index 0) page
            if (tabSelected == 1) {
                // only offer diamonds on colored background mode
                if (config.color_mode == 0) {
                    // if more two+ blocks, then offer a connecting diamond as well
                    if (config.show_user + config.show_path + config.show_git == 1) {
                        showTabs.push_back(tabDmndLeading);
                        tabMessage.push_back(dmndLeadTitle);

                        showTabs.push_back(tabDmndTrailing);
                        tabMessage.push_back(dmndTrailTitle);
                    } else {
                        showTabs.push_back(tabDmndLeading);
                        tabMessage.push_back(dmndLeadTitle);

                        showTabs.push_back(tabDmndConnecting);
                        tabMessage.push_back(dmndConnectTitle);

                        showTabs.push_back(tabDmndTrailing);
                        tabMessage.push_back(dmndTrailTitle);
                    }
                    // only add text color option if colored background mode is selected
                    showTabs.push_back(tabFg);
                    tabMessage.push_back(fgTitle);
                }

                // only add end diamonds & background color option if monochrome mode is selected
                if (config.color_mode == 2) {
                    showTabs.push_back(tabDmndLeading);
                    tabMessage.push_back(dmndLeadTitle);

                    showTabs.push_back(tabDmndTrailing);
                    tabMessage.push_back(dmndTrailTitle);

                    showTabs.push_back(tabBg);
                    tabMessage.push_back(bgTitle);
                }

                if (config.show_user) {
                    showTabs.push_back(tabUserColor);
                    tabMessage.push_back(userColorTitle);
                }
                if (config.show_path) {
                    showTabs.push_back(tabDirColor);
                    tabMessage.push_back(dirColorTitle);
                }
                if (config.show_git) {
                    showTabs.push_back(tabGitColor);
                    tabMessage.push_back(gitColorTitle);
                }

                showTabs.push_back(tabTrPrompt);
                tabMessage.push_back(trPromptTitle);

                showTabs.push_back(tabTitle);
                tabMessage.push_back(titleTitle);

                // rebuild container
                tabContainer = Container::Tab(
                  {
                    showTabs,
                  },
                  &tabSelected);
                container->DetachAllChildren();
                container->Add(tabContainer);
            }

            // stop advancing if at the last page
            if (tabSelected >= showTabs.size() - 1) {
                return true;
            }

            tabSelected++;
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
