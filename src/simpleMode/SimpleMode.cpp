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

    //* Define action lambdas for buttons
    auto quit_action = [=]() { on_quit(); };

    auto end_action = [=, &config]() {
        GenerateJSON(config);
        on_quit();
    };

    auto confirm_action = [=, &config]() {
        //! Pick and add let/right block + other pages
        if (config.tabSelected == 0 && config.showTabs.size() == 1) {
            // stop advancing if no prompt is chosen
            if (!(config.use_left || config.use_right)) {
                return;
            }

            // Add block selection pages for chosen sides
            std::vector<Component> blockCols;
            if (config.use_left) blockCols.push_back(contVec.at("tabChooseBlocks"));
            if (config.use_right) blockCols.push_back(contVec.at("tabChooseRightBlocks"));

            if (!blockCols.empty()) {
                auto combined = Container::Horizontal(blockCols);
                auto combinedRender = Renderer(combined, [combined, contVec, &config] {
                    Elements cols;
                    if (config.use_left) {
                        cols.push_back(vbox({ text("Left Blocks") | bold | hcenter | color(Color::LightSlateGrey) | underlined, contVec.at("tabChooseBlocks")->Render() }) | flex);
                    }
                    if (config.use_left && config.use_right) {
                        cols.push_back(text("   "));
                        cols.push_back(separator() | color(Color::LightSlateGrey));
                        cols.push_back(text("   "));
                    }
                    if (config.use_right) {
                        cols.push_back(vbox({ text("Right Blocks") | bold | hcenter | color(Color::LightSlateGrey) | underlined, contVec.at("tabChooseRightBlocks")->Render() })
                                       | flex);
                    }
                    return hbox(cols);
                });
                config.showTabs.push_back(combinedRender);
                config.tabMessage.push_back(titleVec.at("blocksCombinedTitle"));
            }

            // Add color mode page
            config.showTabs.push_back(contVec.at("tabColorMode"));
            config.tabMessage.push_back(titleVec.at("colorModeTitle"));

            // Rebuild the tab container with the new pages
            auto newTab = Container::Tab(config.showTabs, &config.tabSelected);
            container->DetachAllChildren();
            container->Add(newTab);
        }

        // # Prevent advancing if no blocks are selected on a block-selection page
        if (config.tabSelected == 1) {
            bool leftOk = !config.use_left
                       || (config.show_user || config.show_path || config.show_git || config.show_time || config.show_shell
                           || config.show_executiontime || config.show_battery || config.show_os);
            bool rightOk = !config.use_right
                        || (config.show_user_r || config.show_path_r || config.show_git_r || config.show_time_r || config.show_shell_r
                            || config.show_executiontime_r || config.show_battery_r || config.show_os_r);
            if (!leftOk || !rightOk) {
                return;
            }
        }

        //! Add all remaining tabs
        int colorModeIndex = 2;   // prompt picker is 0, and unified block selector is 1, this is 2

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
                std::vector<Component> leftColorPickers;
                auto addColor = [&](bool show, const string& id, const string& labelText) {
                    if (show) {
                        auto comp = contVec.at(id);
                        auto wrapped
                          = Renderer(comp, [comp, labelText] { return vbox({ text(labelText) | bold | hcenter, text(""), comp->Render() }); });
                        leftColorPickers.push_back(wrapped);
                    }
                };
                addColor(config.show_os, "tabOsColor", "OS Icon Color");
                addColor(config.show_user, "tabUserColor", "User Block Color");
                addColor(config.show_path, "tabDirColor", "Directory Block Color");
                addColor(config.show_git, "tabGitColor", "Git Block Color");
                addColor(config.show_time, "tabTimeColor", "Time Block Color");
                addColor(config.show_shell, "tabShellColor", "Shell Block Color");
                addColor(config.show_executiontime, "tabExecutionTimeColor", "Execution Time Color");
                addColor(config.show_battery, "tabBatteryColor", "Battery Color");

                if (!leftColorPickers.empty()) {
                    auto leftColorContainer = Container::Vertical(leftColorPickers);
                    auto scrollableContainer = Renderer(leftColorContainer, [leftColorContainer] {
                        return leftColorContainer->Render() | vscroll_indicator | yframe | size(HEIGHT, LESS_THAN, 18);
                    });
                    config.showTabs.push_back(scrollableContainer);
                    config.tabMessage.push_back(titleVec.at("leftColorsCombinedTitle"));
                }
            }

            // # Right prompt segment color options
            if (config.use_right) {
                std::vector<Component> rightColorPickers;
                auto addColorR = [&](bool show, const string& id, const string& labelText) {
                    if (show) {
                        auto comp = contVec.at(id);
                        auto wrapped
                          = Renderer(comp, [comp, labelText] { return vbox({ text(labelText) | bold | hcenter, text(""), comp->Render() }); });
                        rightColorPickers.push_back(wrapped);
                    }
                };
                addColorR(config.show_os_r, "tabOsColorRight", "OS Icon Color");
                addColorR(config.show_user_r, "tabUserColorRight", "User Block Color");
                addColorR(config.show_path_r, "tabDirColorRight", "Directory Block Color");
                addColorR(config.show_git_r, "tabGitColorRight", "Git Block Color");
                addColorR(config.show_time_r, "tabTimeColorRight", "Time Block Color");
                addColorR(config.show_shell_r, "tabShellColorRight", "Shell Block Color");
                addColorR(config.show_executiontime_r, "tabExecutionTimeColorRight", "Execution Time Color");
                addColorR(config.show_battery_r, "tabBatteryColorRight", "Battery Color");

                if (!rightColorPickers.empty()) {
                    auto rightColorContainer = Container::Vertical(rightColorPickers);
                    auto scrollableContainer = Renderer(rightColorContainer, [rightColorContainer] {
                        return rightColorContainer->Render() | vscroll_indicator | yframe | size(HEIGHT, LESS_THAN, 18);
                    });
                    config.showTabs.push_back(scrollableContainer);
                    config.tabMessage.push_back(titleVec.at("rightColorsCombinedTitle"));
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
            return;
        }

        config.tabSelected++;
    };

    //* Define button styling (options)
    ButtonOption quit_option;
    quit_option.transform = [](const EntryState& s) {
        auto element = text(s.label) | border;
        if (s.focused) {
            return element | bgcolor(Color::Red) | color(Color::Black) | bold;
        } else {
            return element | color(Color::MediumPurple);
        }
    };

    ButtonOption confirm_option;
    confirm_option.transform = [](const EntryState& s) {
        auto element = text(s.label) | border;
        if (s.focused) {
            return element | bgcolor(Color::Green) | color(Color::Black) | bold;
        } else {
            return element | color(Color::MediumPurple);
        }
    };

    ButtonOption end_option;
    end_option.transform = [](const EntryState& s) {
        auto element = text(s.label) | border;
        if (s.focused) {
            return element | bgcolor(Color::Blue) | color(Color::Black) | bold;
        } else {
            return element | color(Color::MediumPurple);
        }
    };

    //* Define interactive navigation buttons
    auto btn_quit = Button("[q]", quit_action, quit_option);
    auto btn_confirm = Button("[n]", confirm_action, confirm_option);
    auto btn_end = Button("[e]", end_action, end_option);

    auto buttons_vbox = Container::Vertical({
      btn_quit,
      btn_confirm,
      btn_end,
    });

    auto main_layout = Container::Vertical({
      container,
      buttons_vbox,
    });

    //* Helper to vertically center button explanation text to align with 3-line buttons
    auto explanation_line = [](std::string text_str) { return vbox({ text(""), text(text_str), text("") }); };

    // Component to render
    auto toRender = Renderer(main_layout, [container, buttons_vbox, btn_quit, btn_confirm, btn_end, &config, explanation_line] {
        bool is_tab_titles_page = (config.tabSelected < (int) config.tabMessage.size())
                               && (config.tabMessage[config.tabSelected] == "What should the terminal tab titles look like?");
        int expected_count = is_tab_titles_page ? 2 : 3;
        if (buttons_vbox->ChildCount() != expected_count) {
            buttons_vbox->DetachAllChildren();
            if (is_tab_titles_page) {
                buttons_vbox->Add(btn_quit);
                buttons_vbox->Add(btn_end);
            } else {
                buttons_vbox->Add(btn_quit);
                buttons_vbox->Add(btn_confirm);
                buttons_vbox->Add(btn_end);
            }
        }

        auto livePreview = vbox({
            separator() | color(Color::LightSlateGrey),
            text("Preview") | bold | hcenter | color(Color::Yellow),
            buildLivePreview(config),
            separator() | color(Color::LightSlateGrey)
        });

        if (is_tab_titles_page) {
            return vbox({
                window(text(""), text(" " + config.tabMessage[config.tabSelected] + " ") | bold | color(Color::Cyan)) | hcenter | color(Color::LightSlateGrey),
                text(" "),
                container->Render() | hcenter,
                text(" "),
                livePreview,
                hbox({ buttons_vbox->Render(), vbox({ explanation_line(" = Quit the wizard without generating config file"),
                                                      explanation_line(" = End the wizard and generate config file"),
                                                      text("      (will use defaults if all options are not specified)") }) })
                    | hcenter })
            | vcenter;
        } else {
            return vbox({
                window(text(""), text(" " + config.tabMessage[config.tabSelected] + " ") | bold | color(Color::Cyan)) | hcenter | color(Color::LightSlateGrey),
                text(" "),
                container->Render() | hcenter,
                text(" "),
                livePreview,
                hbox({ buttons_vbox->Render(), vbox({ explanation_line(" = Quit the wizard without generating config file"),
                                                      explanation_line(" = Confirm selections & go to the next screen"),
                                                      explanation_line(" = End the wizard and generate config file"),
                                                      text("      (will use defaults if all options are not specified)") }) })
                    | hcenter })
            | vcenter;
        }
    });

    //* Catch keypresses
    auto component = CatchEvent(toRender, [=, &config](Event event) {
        bool is_tab_titles_page = (config.tabSelected < (int) config.tabMessage.size())
                               && (config.tabMessage[config.tabSelected] == "What should the terminal tab titles look like?");
        if (event == Event::Character('q')) {
            quit_action();
            return true;
        }

        if (event == Event::Character('n')) {
            if (!is_tab_titles_page) {
                confirm_action();
                return true;
            }
        }
        if (event == Event::Character('e')) {
            end_action();
            return true;
        }
        return false;
    });

    return component;
}
