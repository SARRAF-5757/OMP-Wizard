#include <fstream>

#include <ftxui/component/component_base.hpp>

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "nlohmann/json.hpp"
#include "../../shared/structs/RGB.hpp"
#include "../../shared/structs/Config.hpp"

using namespace std;
using namespace ftxui;
using json = nlohmann::json;


vector<string> color_mode_choice = 
    { 
        "Transparent Background [No Diamonds]", 
        "Colored Background" 
    };

vector<string> template_mode_choice = 
    { 
        "Default Template",
        "Custom"
    };

vector<string> options_mode_choice = 
    { 
        "Default Options",
        "Custom"
    };

vector<string> trailing_diamond = 
    {
        "None",     // straight edge
        "\ue0b4",   // right half-circle
        "\ue0b0",   // right triangle
        "\ue0bc",   // right upper slant
        "\ue0b8",   // right lower slant
        "\ue0c0",   // right fire
        "\ue0c8",   // right shockwave
        "\ue0c6",   // right big square fade
        "\ue0c4",   // right small square fade
        "Custom"    // input hex code
    };

vector<string> leading_diamond = {
    "None",     // straight edge
    "\ue0b6",   // left half-circle
    "\ue0b2",   // left triangle
    "\ue0ba",   // left lower slant
    "\ue0be",   // left upper slant
    "\ue0c2",   // left fire
    "\ue0ca",   // left shockwave
    "\ue0c7",   // left big square fade
    "\ue0c5",   // left small square fade
    "Custom",   // input hex code
};

vector<string> island_mode_choice = 
    {
        "No",
        "Yes"
    };
//!--------------------------------------Hlper Functions-------------------------------------!//
//@ Helper to convert rgba values to HEX codes
string RGtoHex(int r, int g, int b) {
    stringstream ss;
    ss << "#" << hex << setfill('0') << setw(2) << r << setw(2) << g << setw(2) << b;
    return ss.str();
}

//@ To create tab components for color selector tabs
Component colorPicke(rgb* rgValues, string& hex) {
    auto slider_r = Slider("Red   :", &rgValues->red, 0, 255, 1);
    auto slider_g = Slider("Green :", &rgValues->green, 0, 255, 1);
    auto slider_b = Slider("Blue  :", &rgValues->blue, 0, 255, 1);

    auto layout = Container::Vertical({ slider_r, slider_g, slider_b });

    return Renderer(layout, [=, &hex] {
        auto preview = text("   COLOR   ") 
                            | bgcolor(Color::RGB(rgValues->red, rgValues->green, rgValues->blue))
                            | color(Color::Black);
        hex = RGtoHex(rgValues->red, rgValues->green, rgValues->blue);

        // the size() options are necessary to make it wide enough.
        // or else it shrinks it too much (possibly cuz of hcenter in the main renderer)
        return hbox({ preview, separator(),
                    vbox({ slider_r->Render(), slider_g->Render(), slider_b->Render(), separator(),
                    text("Hex: " + hex) })
                    | size(WIDTH, GREATER_THAN, 30) })
                    | border | size(WIDTH, GREATER_THAN, 30);
    });
}


std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string> menuDisplay(std::string segmentType) {
    // Initialize Stuff
    auto screen = ScreenInteractive::Fullscreen();
    int tabSelected = 0;
    ConfigState config;
    bool tabsAdded = false;
    bool cancel = false;

    string colorModeTitle = "How should the background for " + segmentType + " be colorized?";
    auto tabColorMode = Radiobox({
        .entries = color_mode_choice,
        .selected = &config.color_mode,
    });

    string fgTitle = "Pick a text color for " + segmentType;
    auto tabFg = colorPicke(&config.fg_color, config.color_fg);

    string bgTitle = "Pick a background color for " + segmentType;
    auto tabBg = colorPicke(&config.bg_color, config.color_bg);

    string trailingDmndTitle = "Pick a trailing diamond for " + segmentType;
    auto tabTrailingDmnd = Radiobox({
        .entries = trailing_diamond,
        .selected = &config.dmnd_trailing,
    });;

    string templTitle = "Pick a template for " + segmentType + "\n Note: Custom templates are not guaranteed to work";
    auto tabTempl = Radiobox({
        .entries = template_mode_choice,
        .selected = &config.template_mode,
    });

    string optionTitle = "Pick options for " + segmentType + "\n Note: Custom options are not guaranteed to work";
    auto tabOption = Radiobox({
        .entries = template_mode_choice,
        .selected = &config.option_mode,
    });

    string islandTitle = "Would you like this segment to be an island?";
    auto tabIsland = Radiobox({
        .entries = island_mode_choice,
        .selected = &config.island_mode,
    });

    string leadingDmndTitle = "Pick a leading diamond for " + segmentType;
    auto tabLeadingDmnd = Radiobox({
        .entries = leading_diamond,
        .selected = &config.dmnd_leading,
    });


    //* Vectors to switch contents among tabs (preloaded with first two screens)
    vector<Component> showTabs = { tabFg, tabColorMode };   // Vector of components representing each tab
    vector<string> tabMessage = { fgTitle, colorModeTitle };      // Titles shown on each screens


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
                    vbox({  text("[c] = Cancel adding the segment"),
                            text("[n] = Confirm selections & go to the next screen")})
                            | hcenter })
                            | vcenter;
    });

    //* Catch keypresses
    // To catch 'q' keypress to exit wizard
    auto component = CatchEvent(toRender, [&](Event event) {
        if (event == Event::Character('c')) {
            screen.ExitLoopClosure()();
            cancel = true;
            return true;
        }

        if (event == Event::Character('n')) {
            // only add the rest of the tabs once, while on block selection (index 0) page
            if (tabSelected == 1) {
                // only offer diamonds on colored background mode
                if (config.color_mode == 1) {
                    // only add text color option if colored background mode is selected
                    showTabs.push_back(tabBg);
                    tabMessage.push_back(bgTitle);

                    showTabs.push_back(tabIsland);
                    tabMessage.push_back(islandTitle);
                    
                }
                
                //showTabs.push_back(tabTempl);
                //tabMessage.push_back(templTitle);
                
                //showTabs.push_back(tabOption);
                //tabMessage.push_back(optionTitle);
                
                // rebuild container
                tabContainer = Container::Tab(
                {
                    showTabs,
                },
                &tabSelected);
                container->DetachAllChildren();
                container->Add(tabContainer);
            } else if (tabSelected == 3 && showTabs[3] == tabIsland){
                if(config.island_mode == 1 && config.color_mode == 1){
                    showTabs.push_back(tabLeadingDmnd);
                    tabMessage.push_back(leadingDmndTitle);
                }
                if(config.color_mode == 1){
                    showTabs.push_back(tabTrailingDmnd);
                    tabMessage.push_back(trailingDmndTitle);
                }
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
                screen.ExitLoopClosure()();
                return true;
            }

            tabSelected++;
        }
        return false;
    });

    // Loop and render component on the screen
    screen.Loop(component);
    if(cancel){
        return std::make_tuple("cancel", "cancel", "cancel", "cancel", "cancel", "cancel", "cancel");
    } else if(config.color_mode == 1){
        config.color_bg = RGtoHex(config.bg_color.red, config.bg_color.green, config.bg_color.blue);
    }
    return std::make_tuple( RGtoHex(config.fg_color.red, config.fg_color.green, config.fg_color.blue), 
                            config.color_bg, 
                            island_mode_choice[config.island_mode],
                            leading_diamond[config.dmnd_leading],
                            trailing_diamond[config.dmnd_trailing],
                            template_mode_choice[config.template_mode], 
                            options_mode_choice[config.option_mode]
                        );
}
