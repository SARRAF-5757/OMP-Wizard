#include "simpleMode.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "helpers/genPageContents.hpp"
#include "helpers/jsonGenerate.hpp"


using namespace std;
using namespace ftxui;

Component MakeSimpleMode(ConfigState& config, function<void()> on_quit) {
    //* Prepare data
    // Generate map of page content for fast access
    unordered_map<string, Component> contVec = genPageContents(config);
    unordered_map<string, string> titleVec = genPageTitles();

    // Vectors to switch contents among tabs (preloaded with first two screens)
    config.showTabs = { contVec["tabChooseBlocks"], contVec["tabColorMode"] };     // Vector of components representing each tab
    config.tabMessage = { titleVec["blocksTitle"], titleVec["colorModeTitle"] };   // Titles shown on each screens

    //* Setup UI
    // Container of tabs
    auto tabContainer = Container::Tab(config.showTabs, &config.tabSelected);

    // Container for the viewed tab (tab swapped out in this container)
    auto container = Container::Vertical({
      tabContainer,
    });

    // Component to render
    auto toRender = Renderer(container, [container, &config] {
        return vbox(
                 { text(config.tabMessage[config.tabSelected]) | hcenter, text(" "), container->Render() | hcenter, text(" "), text(" "),
                   vbox({ text("[q] = Quit the wizard without generating config file"), text("[n] = Confirm selections & go to the next screen"),
                          text("[e] = End the wizard and generate config file"), text("      (will use defaults if all options are not specified)") })
                     | hcenter })
             | vcenter;
    });

    //* Catch keypresses
    auto component = CatchEvent(toRender, [=, &config](Event event) {
        // To catch 'q' keypress to exit wizard
        if (event == Event::Character('q')) {
            on_quit();
            return true;
        }

        if (event == Event::Character('n')) {
            // Add the rest of the tabs only once [while on block selection (index 0) page]
            if (config.tabSelected == 1) {
                // # only offer diamonds & color options on full color mode
                if (config.color_mode == 0) {
                    if (config.show_user + config.show_path + config.show_git == 1) {
                        config.showTabs.push_back(contVec.at("tabDmndLeading"));
                        config.tabMessage.push_back(titleVec.at("dmndLeadTitle"));

                        config.showTabs.push_back(contVec.at("tabDmndTrailing"));
                        config.tabMessage.push_back(titleVec.at("dmndTrailTitle"));
                    } else {
                        // if two+ blocks, then offer a connecting diamond as well
                        config.showTabs.push_back(contVec.at("tabDmndLeading"));
                        config.tabMessage.push_back(titleVec.at("dmndLeadTitle"));

                        config.showTabs.push_back(contVec.at("tabDmndConnecting"));
                        config.tabMessage.push_back(titleVec.at("dmndConnectTitle"));

                        config.showTabs.push_back(contVec.at("tabDmndTrailing"));
                        config.tabMessage.push_back(titleVec.at("dmndTrailTitle"));
                    }
                    config.showTabs.push_back(contVec.at("tabFg"));
                    config.tabMessage.push_back(titleVec.at("fgTitle"));
                }

                // # only offer end diamonds & background color option on monochrome mode
                if (config.color_mode == 2) {
                    config.showTabs.push_back(contVec.at("tabDmndLeading"));
                    config.tabMessage.push_back(titleVec.at("dmndLeadTitle"));

                    config.showTabs.push_back(contVec.at("tabDmndTrailing"));
                    config.tabMessage.push_back(titleVec.at("dmndTrailTitle"));

                    config.showTabs.push_back(contVec.at("tabBg"));
                    config.tabMessage.push_back(titleVec.at("bgTitle"));
                }

                // # block color options
                if (config.show_user) {
                    config.showTabs.push_back(contVec.at("tabUserColor"));
                    config.tabMessage.push_back(titleVec.at("userColorTitle"));
                }
                if (config.show_path) {
                    config.showTabs.push_back(contVec.at("tabDirColor"));
                    config.tabMessage.push_back(titleVec.at("dirColorTitle"));
                }
                if (config.show_git) {
                    config.showTabs.push_back(contVec.at("tabGitColor"));
                    config.tabMessage.push_back(titleVec.at("gitColorTitle"));
                }

                // # misc options
                config.showTabs.push_back(contVec.at("tabTrPrompt"));
                config.tabMessage.push_back(titleVec.at("trPromptTitle"));

                config.showTabs.push_back(contVec.at("tabTitle"));
                config.tabMessage.push_back(titleVec.at("titleTitle"));

                // rebuild container
                auto newTab = Container::Tab(config.showTabs, &config.tabSelected);
                container->DetachAllChildren();
                container->Add(newTab);
            }

            //* Prevent changing tab under certain conditions
            if (config.tabSelected == 0) {   // if no block is selected
                if (!(config.show_user || config.show_path || config.show_git)) {
                    return true;
                }
            }

            if (config.tabSelected >= config.showTabs.size() - 1) {   // if at the last page
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

    return component;
}
