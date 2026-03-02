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

    // Start with just the prompt picker page
    config.showTabs = { contVec["tabPickPrompts"] };
    config.tabMessage = { titleVec["promptPickTitle"] };

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
        if (event == Event::Character('q')) {
            on_quit();
            return true;
        }

        if (event == Event::Character('n')) {
            //! Pick and add let/right block + other pages
            if (config.tabSelected == 0 && config.showTabs.size() == 1) {
                // stop advancing if no prompt is chosen
                if (!(config.use_left || config.use_right)) {
                    return true;
                }

                // Add block selection pages for chosen sides
                if (config.use_left) {
                    config.showTabs.push_back(contVec.at("tabChooseBlocks"));
                    config.tabMessage.push_back(titleVec.at("blocksTitle"));
                }
                if (config.use_right) {
                    config.showTabs.push_back(contVec.at("tabChooseRightBlocks"));
                    config.tabMessage.push_back(titleVec.at("rightBlocksTitle"));
                }

                // Add color mode page
                config.showTabs.push_back(contVec.at("tabColorMode"));
                config.tabMessage.push_back(titleVec.at("colorModeTitle"));

                // Rebuild the tab container with the new pages
                auto newTab = Container::Tab(config.showTabs, &config.tabSelected);
                container->DetachAllChildren();
                container->Add(newTab);
            }

            //* Prevent advancing if no blocks are selected on a block-selection page
            // Left block selection page is at index 1 (only when use_left is true)
            if (config.use_left && config.tabSelected == 1) {
                if (!(config.show_user || config.show_path || config.show_git || config.show_time || config.show_shell)) {
                    return true;
                }
            }
            // Right block selection page index depends on whether left was also chosen
            int rightBlocksIndex = config.use_left ? 2 : 1;
            if (config.use_right && config.tabSelected == rightBlocksIndex) {
                if (!(config.show_user_r || config.show_path_r || config.show_git_r || config.show_time_r || config.show_shell_r)) {
                    return true;
                }
            }

            //! Add all remaining tabs
            int colorModeIndex = 1 + (config.use_left ? 1 : 0) + (config.use_right ? 1 : 0);   // Figure out where the color mode page ended up

            if (!config.tabsAdded && config.tabSelected == colorModeIndex) {
                config.tabsAdded = true;

                // # Diamond options (left prompt only)
                if (config.use_left) {
                    int leftCount = (int) config.show_user + (int) config.show_path + (int) config.show_git;

                    if (config.color_mode == 0) {
                        if (leftCount == 1) {
                            config.showTabs.push_back(contVec.at("tabDmndLeading"));
                            config.tabMessage.push_back(titleVec.at("dmndLeadTitle"));
                            config.showTabs.push_back(contVec.at("tabDmndTrailing"));
                            config.tabMessage.push_back(titleVec.at("dmndTrailTitle"));
                        } else {
                            config.showTabs.push_back(contVec.at("tabDmndLeading"));
                            config.tabMessage.push_back(titleVec.at("dmndLeadTitle"));
                            config.showTabs.push_back(contVec.at("tabDmndConnecting"));
                            config.tabMessage.push_back(titleVec.at("dmndConnectTitle"));
                            config.showTabs.push_back(contVec.at("tabDmndTrailing"));
                            config.tabMessage.push_back(titleVec.at("dmndTrailTitle"));
                        }
                    }
                    if (config.color_mode == 2) {
                        config.showTabs.push_back(contVec.at("tabDmndLeading"));
                        config.tabMessage.push_back(titleVec.at("dmndLeadTitle"));
                        config.showTabs.push_back(contVec.at("tabDmndTrailing"));
                        config.tabMessage.push_back(titleVec.at("dmndTrailTitle"));
                    }
                }

                // # Global fg/bg color options (used by both prompts)
                if (config.color_mode == 0) {
                    config.showTabs.push_back(contVec.at("tabFg"));
                    config.tabMessage.push_back(titleVec.at("fgTitle"));
                }
                if (config.color_mode == 2) {
                    config.showTabs.push_back(contVec.at("tabBg"));
                    config.tabMessage.push_back(titleVec.at("bgTitle"));
                }

                // # Left prompt segment color options
                if (config.use_left) {
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
                    if (config.show_time) {
                        config.showTabs.push_back(contVec.at("tabTimeColor"));
                        config.tabMessage.push_back(titleVec.at("timeColorTitle"));
                    }
                    if (config.show_shell) {
                        config.showTabs.push_back(contVec.at("tabShellColor"));
                        config.tabMessage.push_back(titleVec.at("shellColorTitle"));
                    }
                }

                // # Right prompt segment color options
                if (config.use_right) {
                    if (config.show_user_r) {
                        config.showTabs.push_back(contVec.at("tabUserColorRight"));
                        config.tabMessage.push_back(titleVec.at("userColorRightTitle"));
                    }
                    if (config.show_path_r) {
                        config.showTabs.push_back(contVec.at("tabDirColorRight"));
                        config.tabMessage.push_back(titleVec.at("dirColorRightTitle"));
                    }
                    if (config.show_git_r) {
                        config.showTabs.push_back(contVec.at("tabGitColorRight"));
                        config.tabMessage.push_back(titleVec.at("gitColorRightTitle"));
                    }
                    if (config.show_time_r) {
                        config.showTabs.push_back(contVec.at("tabTimeColorRight"));
                        config.tabMessage.push_back(titleVec.at("timeColorRightTitle"));
                    }
                    if (config.show_shell_r) {
                        config.showTabs.push_back(contVec.at("tabShellColorRight"));
                        config.tabMessage.push_back(titleVec.at("shellColorRightTitle"));
                    }
                }

                // # Misc options (always added)
                config.showTabs.push_back(contVec.at("tabTrPrompt"));
                config.tabMessage.push_back(titleVec.at("trPromptTitle"));
                config.showTabs.push_back(contVec.at("tabTitle"));
                config.tabMessage.push_back(titleVec.at("titleTitle"));

                // Rebuild the tab container with all pages
                auto newTab = Container::Tab(config.showTabs, &config.tabSelected);
                container->DetachAllChildren();
                container->Add(newTab);
            }

            // Don't advance past the last page
            if (config.tabSelected >= (int) config.showTabs.size() - 1) {
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
