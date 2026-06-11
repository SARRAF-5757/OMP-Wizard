#include <fstream>

#include <ftxui/component/component_base.hpp>

#include "../../shared/helpers/colorPicker.hpp"
#include "../../shared/helpers/convertColors.hpp"
#include "../../shared/structs/Config.hpp"
#include "../../shared/structs/Constants.hpp"
#include "../../shared/structs/RGB.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "nlohmann/json.hpp"

using namespace ftxui;
using json = nlohmann::json;

void initAdvancedColorDefaults(ConfigState& config) {
    config.fg_color = { 255, 255, 255 };
    config.bg_color = { 0, 0, 255 };
    config.color_fg = "#FFFFFF";
    config.color_bg = "#0000FF";
}

std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string> menuDisplay(
  std::string segmentType, bool isFirstSegment, bool prevIsland) {
    // Initialize Stuff
    auto screen = ScreenInteractive::Fullscreen();
    int tabSelected = 0;
    ConfigState config;
    initAdvancedColorDefaults(config);
    bool tabsAdded = false;
    bool cancel = false;

    string colorModeTitle = "How should the background for " + segmentType + " be colorized?";
    auto tabColorMode = Radiobox({
      .entries = adv_color_mode_choice,
      .selected = &config.color_mode,
    });

    string fgTitle = "Pick a text color for " + segmentType;
    auto tabFg = colorPicker(&config.fg_color, config.color_fg);

    string bgTitle = "Pick a background color for " + segmentType;
    auto tabBg = colorPicker(&config.bg_color, config.color_bg);

    string trailingDmndTitle = "Pick a trailing diamond for " + segmentType;
    auto tabTrailingDmnd = Radiobox({
      .entries = trailing_diamonds,
      .selected = &config.dmnd_trailing,
    });
    ;

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
      .entries = leading_diamonds,
      .selected = &config.dmnd_leading,
    });


    //* Vectors to switch contents among tabs (preloaded with first two screens)
    vector<Component> showTabs = { tabFg, tabColorMode };      // Vector of components representing each tab
    vector<string> tabMessage = { fgTitle, colorModeTitle };   // Titles shown on each screens


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
        return vbox({ text(tabMessage[tabSelected]) | hcenter, text(" "), tabContainer->Render() | hcenter, text(" "), text(" "),
                      vbox({ text("[c] = Cancel adding the segment"), text("[n] = Confirm selections & go to the next screen") }) | hcenter })
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

                // showTabs.push_back(tabTempl);
                // tabMessage.push_back(templTitle);

                // showTabs.push_back(tabOption);
                // tabMessage.push_back(optionTitle);

                // rebuild container
                tabContainer = Container::Tab(
                  {
                    showTabs,
                  },
                  &tabSelected);
                container->DetachAllChildren();
                container->Add(tabContainer);
            } else if (tabSelected == 3 && (showTabs[3] == tabIsland)) {
                if ((config.island_mode == 1 || prevIsland || isFirstSegment) && config.color_mode == 1) {
                    showTabs.push_back(tabLeadingDmnd);
                    tabMessage.push_back(leadingDmndTitle);
                }
                if (config.color_mode == 1) {
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
    if (cancel) {
        return std::make_tuple("cancel", "cancel", "cancel", "cancel", "cancel", "cancel", "cancel");
    } else if (config.color_mode == 1) {
        config.color_bg = RGBtoHex(config.bg_color.red, config.bg_color.green, config.bg_color.blue);
    }
    return std::make_tuple(RGBtoHex(config.fg_color.red, config.fg_color.green, config.fg_color.blue), config.color_bg,
                           island_mode_choice[config.island_mode], leading_diamonds[config.dmnd_leading], trailing_diamonds[config.dmnd_trailing],
                           template_mode_choice[config.template_mode], options_mode_choice[config.option_mode]);
}

std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string> menuDisplayEdit(
  std::string segmentType, bool isFirstSegment, bool prevIsland, bool currIsland) {
    auto screen = ScreenInteractive::Fullscreen();
    int tabSelected = 0;
    ConfigState config;
    initAdvancedColorDefaults(config);
    bool cancel = false;

    string colorModeTitle = "How should the background for " + segmentType + " be colorized?";
    auto tabColorMode = Radiobox({
      .entries = adv_color_mode_choice,
      .selected = &config.color_mode,
    });

    string fgTitle = "Pick a text color for " + segmentType;
    auto tabFg = colorPicker(&config.fg_color, config.color_fg);

    string bgTitle = "Pick a background color for " + segmentType;
    auto tabBg = colorPicker(&config.bg_color, config.color_bg);

    string trailingDmndTitle = "Pick a trailing diamond for " + segmentType;
    auto tabTrailingDmnd = Radiobox({
      .entries = trailing_diamonds,
      .selected = &config.dmnd_trailing,
    });

    string leadingDmndTitle = "Pick a leading diamond for " + segmentType;
    auto tabLeadingDmnd = Radiobox({
      .entries = leading_diamonds,
      .selected = &config.dmnd_leading,
    });

    vector<Component> showTabs = { tabFg, tabColorMode };
    vector<string> tabMessage = { fgTitle, colorModeTitle };

    auto tabContainer = Container::Tab({ showTabs }, &tabSelected);

    auto container = Container::Vertical({ tabContainer });

    auto toRender = Renderer(container, [&] {
        return vbox({ text(tabMessage[tabSelected]) | hcenter, text(" "), tabContainer->Render() | hcenter, text(" "), text(" "),
                      vbox({ text("[c] = Cancel editing the segment"), text("[n] = Confirm selections & go to the next screen") }) | hcenter })
             | vcenter;
    });

    auto component = CatchEvent(toRender, [&](Event event) {
        if (event == Event::Character('c')) {
            screen.ExitLoopClosure()();
            cancel = true;
            return true;
        }

        if (event == Event::Character('n')) {
            if (tabSelected == 1 && config.color_mode == 1) {
                showTabs.push_back(tabBg);
                tabMessage.push_back(bgTitle);

                if (isFirstSegment || prevIsland || currIsland) {
                    showTabs.push_back(tabLeadingDmnd);
                    tabMessage.push_back(leadingDmndTitle);
                }

                showTabs.push_back(tabTrailingDmnd);
                tabMessage.push_back(trailingDmndTitle);

                tabContainer = Container::Tab({ showTabs }, &tabSelected);
                container->DetachAllChildren();
                container->Add(tabContainer);
            }

            if (tabSelected >= showTabs.size() - 1) {
                screen.ExitLoopClosure()();
                return true;
            }

            tabSelected++;
        }
        return false;
    });

    screen.Loop(component);
    if (cancel) {
        return std::make_tuple("cancel", "cancel", "cancel", "cancel", "cancel", "cancel", "cancel");
    }
    if (config.color_mode == 1) {
        config.color_bg = RGBtoHex(config.bg_color.red, config.bg_color.green, config.bg_color.blue);
    }
    return std::make_tuple(RGBtoHex(config.fg_color.red, config.fg_color.green, config.fg_color.blue), config.color_bg,
                           "", leading_diamonds[config.dmnd_leading], trailing_diamonds[config.dmnd_trailing], "", "");
}
