#include "SimpleMode.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "helpers/colorPicker.hpp"
#include "helpers/jsonGenerate.hpp"
#include "structs/Diamonds.hpp"


using namespace std;
using namespace ftxui;

Component MakeSimpleMode(ConfigState& config, function<void()> on_quit) {
    // Reset Stuff
    config.showTabs.clear();
    config.tabMessage.clear();
    config.tabSelected = 0;

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
    config.showTabs = { tabChooseBlocks, tabColorMode };   // Vector of components representing each tab
    config.tabMessage = { blocksTitle, colorModeTitle };   // Titles shown on each screens


    // Container of the differing tab blocks
    auto tabContainer = Container::Tab(config.showTabs, &config.tabSelected);

    // Container representing the viewed tab
    auto container = Container::Vertical({
      tabContainer,
    });

    // Component to render
    auto toRender = Renderer(container, [container, &config] {
        // Display components vertically
        return vbox({ text(config.tabMessage[config.tabSelected]) | hcenter, text(" "), container->Render() | hcenter,
                      text(" "), text(" "),
                      vbox({ text("[q] = Quit the wizard without generating config file"),
                             text("[n] = Confirm selections & go to the next screen"),
                             text("[e] = End the wizard and generate config file"),
                             text("      (will use defaults if all options are not specified)") })
                        | hcenter })
             | vcenter;
    });

    //* Catch keypresses
    // To catch 'q' keypress to exit wizard
    auto component = CatchEvent(toRender, [=, &config](Event event) {
        if (event == Event::Character('q')) {
            on_quit();
            return true;
        }

        if (event == Event::Character('n')) {
            // don't advance if no block is selected
            if (config.tabSelected == 0) {
                // don't advance if no block is selected
                if (!(config.show_user || config.show_path || config.show_git)) {
                    return true;
                }
            }

            // only add the rest of the tabs once, while on block selection (index 0) page
            if (config.tabSelected == 1) {
                // only offer diamonds on colored background mode
                if (config.color_mode == 0) {
                    // if more two+ blocks, then offer a connecting diamond as well
                    if (config.show_user + config.show_path + config.show_git == 1) {
                        config.showTabs.push_back(tabDmndLeading);
                        config.tabMessage.push_back(dmndLeadTitle);

                        config.showTabs.push_back(tabDmndTrailing);
                        config.tabMessage.push_back(dmndTrailTitle);
                    } else {
                        config.showTabs.push_back(tabDmndLeading);
                        config.tabMessage.push_back(dmndLeadTitle);

                        config.showTabs.push_back(tabDmndConnecting);
                        config.tabMessage.push_back(dmndConnectTitle);

                        config.showTabs.push_back(tabDmndTrailing);
                        config.tabMessage.push_back(dmndTrailTitle);
                    }
                    // only add text color option if colored background mode is selected
                    config.showTabs.push_back(tabFg);
                    config.tabMessage.push_back(fgTitle);
                }

                // only add end diamonds & background color option if monochrome mode is selected
                if (config.color_mode == 2) {
                    config.showTabs.push_back(tabDmndLeading);
                    config.tabMessage.push_back(dmndLeadTitle);

                    config.showTabs.push_back(tabDmndTrailing);
                    config.tabMessage.push_back(dmndTrailTitle);

                    config.showTabs.push_back(tabBg);
                    config.tabMessage.push_back(bgTitle);
                }

                if (config.show_user) {
                    config.showTabs.push_back(tabUserColor);
                    config.tabMessage.push_back(userColorTitle);
                }
                if (config.show_path) {
                    config.showTabs.push_back(tabDirColor);
                    config.tabMessage.push_back(dirColorTitle);
                }
                if (config.show_git) {
                    config.showTabs.push_back(tabGitColor);
                    config.tabMessage.push_back(gitColorTitle);
                }

                config.showTabs.push_back(tabTrPrompt);
                config.tabMessage.push_back(trPromptTitle);

                config.showTabs.push_back(tabTitle);
                config.tabMessage.push_back(titleTitle);

                // rebuild container
                auto newTab = Container::Tab(config.showTabs, &config.tabSelected);
                container->DetachAllChildren();
                container->Add(newTab);
            }

            // stop advancing if at the last page
            if (config.tabSelected >= config.showTabs.size() - 1) {
                return true;
            }

            config.tabSelected++;
        }
        if (event == Event::Character('e')) {
            GenerateJSON(config);
            on_quit();
            return true;
        }
        return false;
    });

    // Return component to main
    return component;

    return 0;
}
