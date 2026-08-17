#include "../../shared/helpers/colorPicker.hpp"
#include "../../shared/structs/Config.hpp"
#include "../../shared/structs/Constants.hpp"
#include "ftxui/component/component.hpp"


//* Content inside each tab
unordered_map<std::string, ftxui::Component> genPageContents(ConfigState& config) {
    unordered_map<std::string, ftxui::Component> contVec;

    //* Define checkbox/radiobox options with vertical padding
    CheckboxOption checkbox_option = CheckboxOption::Simple();
    auto default_checkbox_transform = checkbox_option.transform;
    checkbox_option.transform = [default_checkbox_transform](const EntryState& s) {
        return vbox({ text(""), default_checkbox_transform(s) });
    };

    RadioboxOption radiobox_option = RadioboxOption::Simple();
    auto default_radiobox_transform = radiobox_option.transform;
    radiobox_option.transform = [default_radiobox_transform](const EntryState& s) {
        return vbox({ text(""), default_radiobox_transform(s) });
    };

    auto tabPickPrompts = Container::Vertical({
      Checkbox("Left Block", &config.use_left, checkbox_option),
      Checkbox("Right Block", &config.use_right, checkbox_option),
    });
    auto tabChooseBlocks = Container::Vertical({
      Checkbox("OS Icon", &config.show_os, checkbox_option),
      Checkbox("User Name", &config.show_user, checkbox_option),
      Checkbox("Directory Path", &config.show_path, checkbox_option),
      Checkbox("Git Status", &config.show_git, checkbox_option),
      Checkbox("Time", &config.show_time, checkbox_option),
      Checkbox("Shell Name", &config.show_shell, checkbox_option),
      Checkbox("Execution Time", &config.show_executiontime, checkbox_option),
      Checkbox("Battery", &config.show_battery, checkbox_option),
    });
    auto tabChooseRightBlocks = Container::Vertical({
      Checkbox("OS Icon", &config.show_os_r, checkbox_option),
      Checkbox("User Name", &config.show_user_r, checkbox_option),
      Checkbox("Directory Path", &config.show_path_r, checkbox_option),
      Checkbox("Git Status", &config.show_git_r, checkbox_option),
      Checkbox("Time", &config.show_time_r, checkbox_option),
      Checkbox("Shell Name", &config.show_shell_r, checkbox_option),
      Checkbox("Execution Time", &config.show_executiontime_r, checkbox_option),
      Checkbox("Battery", &config.show_battery_r, checkbox_option),
    });

    // MenuOption to set up labels to be shown from string identifiers
    MenuOption order_menu_option;
    order_menu_option.entries_option.transform = [](const EntryState& state) {
        std::string label = state.label;
        if (label == "os")
            label = "OS Icon";
        else if (label == "session")
            label = "User Name";
        else if (label == "path")
            label = "Directory Path";
        else if (label == "git")
            label = "Git Status";
        else if (label == "time")
            label = "Time";
        else if (label == "shell")
            label = "Shell Name";
        else if (label == "executiontime")
            label = "Execution Time";
        else if (label == "battery")
            label = "Battery";

        // use > to indicate the currently selected item
        auto prefix = state.active ? text("> ") : text("  ");
        auto element = hbox({ prefix, text(label) });
        if (state.focused)
            element = element | inverted;
        return element;
    };

    // Left prompt reordering component
    auto left_selected = std::make_shared<int>(0);
    auto left_menu = Menu(&config.left_order, left_selected.get(), order_menu_option);

    auto left_reorder = CatchEvent(left_menu, [&config, left_selected](Event event) {
        // Pressing 'u' swaps the currently selected block with the one above it
        if (event == Event::Character('u') || event == Event::Character('U')) {
            if (*left_selected > 0) {
                std::swap(config.left_order[*left_selected], config.left_order[*left_selected - 1]);
                (*left_selected)--;
                return true;
            }
        }
        // Pressing 'd' swaps the currently selected block with the one below it
        if (event == Event::Character('d') || event == Event::Character('D')) {
            if (*left_selected < (int) config.left_order.size() - 1) {
                std::swap(config.left_order[*left_selected], config.left_order[*left_selected + 1]);
                (*left_selected)++;
                return true;
            }
        }
        return false;
    });

    // Right prompt reordering component
    auto right_selected = std::make_shared<int>(0);
    auto right_menu = Menu(&config.right_order, right_selected.get(), order_menu_option);

    auto right_reorder = CatchEvent(right_menu, [&config, right_selected](Event event) {
        // Pressing 'u' swaps the currently selected block with the one above it
        if (event == Event::Character('u') || event == Event::Character('U')) {
            if (*right_selected > 0) {
                std::swap(config.right_order[*right_selected], config.right_order[*right_selected - 1]);
                (*right_selected)--;
                return true;
            }
        }
        // Pressing 'd' swaps the currently selected block with the one below it
        if (event == Event::Character('d') || event == Event::Character('D')) {
            if (*right_selected < (int) config.right_order.size() - 1) {
                std::swap(config.right_order[*right_selected], config.right_order[*right_selected + 1]);
                (*right_selected)++;
                return true;
            }
        }
        return false;
    });

    auto order_container = Container::Horizontal({ left_reorder, right_reorder });

    auto order_renderer = Renderer(order_container, [left_reorder, right_reorder, &config] {
        Elements cols;
        if (config.use_left && !config.left_order.empty()) {
            cols.push_back(vbox({ text("Left Prompt") | bold | hcenter | underlined, text(""), left_reorder->Render() }) | flex);
        }
        if (config.use_left && config.use_right) {
            cols.push_back(text("   "));
            cols.push_back(separator());
            cols.push_back(text("   "));
        }
        if (config.use_right && !config.right_order.empty()) {
            cols.push_back(vbox({ text("Right Prompt") | bold | hcenter | underlined, text(""), right_reorder->Render() }) | flex);
        }
        return vbox({ text("Select an item and press 'u' to move up, 'd' to move down.") | dim | hcenter, text(""), hbox(cols) });
    });

    contVec["tabPickPrompts"] = (tabPickPrompts);
    contVec["tabChooseBlocks"] = (tabChooseBlocks);
    contVec["tabChooseRightBlocks"] = (tabChooseRightBlocks);
    contVec["tabOrderBlocks"] = (order_renderer);
    contVec["tabColorMode"] = Radiobox(RadioboxOption { color_mode_choice, &config.color_mode, radiobox_option.transform });
    contVec["tabTrPrompt"] = Radiobox(RadioboxOption { boolean_choice, &config.tr_prompt, radiobox_option.transform });
    contVec["tabTitle"] = Radiobox(RadioboxOption { title_choices, &config.title_mode, radiobox_option.transform });
    contVec["tabDmndLeading"] = Radiobox(RadioboxOption { leading_diamonds, &config.dmnd_leading, radiobox_option.transform });
    contVec["tabDmndConnecting"] = Radiobox(RadioboxOption { trailing_diamonds, &config.dmnd_connecting, radiobox_option.transform });
    contVec["tabDmndTrailing"] = Radiobox(RadioboxOption { trailing_diamonds, &config.dmnd_trailing, radiobox_option.transform });
    contVec["tabFg"] = colorPicker(&config.fg_color, config.color_fg);
    contVec["tabBg"] = colorPicker(&config.bg_color, config.color_bg);
    contVec["tabUserColor"] = colorPicker(&config.user_color, config.color_user);
    contVec["tabDirColor"] = colorPicker(&config.path_color, config.color_path);
    contVec["tabGitColor"] = colorPicker(&config.git_color, config.color_git);
    contVec["tabTimeColor"] = colorPicker(&config.time_color, config.color_time);
    contVec["tabShellColor"] = colorPicker(&config.shell_color, config.color_shell);
    contVec["tabUserColorRight"] = colorPicker(&config.user_color_r, config.color_user_r);
    contVec["tabDirColorRight"] = colorPicker(&config.path_color_r, config.color_path_r);
    contVec["tabGitColorRight"] = colorPicker(&config.git_color_r, config.color_git_r);
    contVec["tabTimeColorRight"] = colorPicker(&config.time_color_r, config.color_time_r);
    contVec["tabShellColorRight"] = colorPicker(&config.shell_color_r, config.color_shell_r);
    contVec["tabExecutionTimeColor"] = colorPicker(&config.executiontime_color, config.color_executiontime);
    contVec["tabBatteryColor"] = colorPicker(&config.battery_color, config.color_battery);
    contVec["tabOsColor"] = colorPicker(&config.os_color, config.color_os);
    contVec["tabExecutionTimeColorRight"] = colorPicker(&config.executiontime_color_r, config.color_executiontime_r);
    contVec["tabBatteryColorRight"] = colorPicker(&config.battery_color_r, config.color_battery_r);
    contVec["tabOsColorRight"] = colorPicker(&config.os_color_r, config.color_os_r);

    return contVec;
}

//* Titles for each tab
unordered_map<std::string, std::string> genPageTitles() {
    unordered_map<std::string, std::string> titleVec;

    titleVec["promptPickTitle"] = "Which prompt sides do you want to configure?";
    titleVec["blocksCombinedTitle"] = "Choose Components for your Prompts";
    titleVec["orderBlocksTitle"] = "Order Your Prompt Segments";
    titleVec["colorModeTitle"] = "How should the prompt(s) be colorized?";
    titleVec["trPromptTitle"] = "Transient Prompt: Do you want to reduce clutter by shrinking your prompt after you hit Enter?";
    titleVec["titleTitle"] = "What should the terminal tab titles look like?";
    titleVec["dmndLeadTitle"] = "Choose Leading Diamond to Show in your Prompt";
    titleVec["dmndConnectTitle"] = "Choose Connecting Diamond to Show in your Prompt";
    titleVec["dmndTrailTitle"] = "Choose Trailing Diamond to Show in your Prompt";
    titleVec["fgTitle"] = "[Both Prompts] Pick a text color";
    titleVec["bgTitle"] = "[Both Prompts] Pick a background color";
    titleVec["leftColorsCombinedTitle"] = "[Left Prompt] Pick Colors for your Selected Blocks (scroll to reveal more)";
    titleVec["rightColorsCombinedTitle"] = "[Right Prompt] Pick Colors for your Selected Blocks scroll to reveal more)";

    return titleVec;
}

// Live preview stuff
struct PreviewSegment {
    std::string text;
    rgb bgColor;
};

static ftxui::Element renderSegments(const std::vector<PreviewSegment>& segments, const ConfigState& config) {
    if (segments.empty()) return ftxui::text("");

    ftxui::Elements elements;

    auto globalFg = ftxui::Color::RGB(config.fg_color.red, config.fg_color.green, config.fg_color.blue);
    auto globalBg = ftxui::Color::RGB(config.bg_color.red, config.bg_color.green, config.bg_color.blue);

    std::string leadStr = (config.dmnd_leading == 0) ? "" : leading_diamonds[config.dmnd_leading];
    std::string connectStr = (config.dmnd_connecting == 0) ? "" : trailing_diamonds[config.dmnd_connecting];
    std::string trailStr = (config.dmnd_trailing == 0) ? "" : trailing_diamonds[config.dmnd_trailing];

    for (size_t i = 0; i < segments.size(); ++i) {
        auto bgRGB = ftxui::Color::RGB(segments[i].bgColor.red, segments[i].bgColor.green, segments[i].bgColor.blue);
        ftxui::Color textCol, bgCol, diamondCol;

        if (config.color_mode == 0) {   // colored bg
            textCol = globalFg;
            bgCol = bgRGB;
            diamondCol = bgRGB;
        } else if (config.color_mode == 1) {   // transparent
            textCol = bgRGB;
            bgCol = ftxui::Color::Default;
            diamondCol = bgRGB;
        } else {   // monochrome
            textCol = bgRGB;
            bgCol = globalBg;
            diamondCol = globalBg;
        }

        // Add leading diamond
        if (i == 0 && !leadStr.empty()) {
            if (config.color_mode == 0) {
                elements.push_back(ftxui::text(leadStr) | ftxui::color(bgRGB) | ftxui::bgcolor(ftxui::Color::Default));
            } else {
                elements.push_back(ftxui::text(leadStr) | ftxui::color(diamondCol) | ftxui::bgcolor(ftxui::Color::Default));
            }
        } else if (i > 0 && !connectStr.empty()) {
            if (config.color_mode == 0) {
                auto prevBgRGB = ftxui::Color::RGB(segments[i - 1].bgColor.red, segments[i - 1].bgColor.green, segments[i - 1].bgColor.blue);
                elements.push_back(ftxui::text(connectStr) | ftxui::color(prevBgRGB) | ftxui::bgcolor(bgRGB));
            } else {
                elements.push_back(ftxui::text(connectStr) | ftxui::color(textCol) | ftxui::bgcolor(bgCol));
            }
        }

        elements.push_back(ftxui::text(segments[i].text) | ftxui::color(textCol) | ftxui::bgcolor(bgCol));

        // Add trailing diamond
        if (i == segments.size() - 1 && !trailStr.empty()) {
            if (config.color_mode == 0) {
                elements.push_back(ftxui::text(trailStr) | ftxui::color(bgRGB) | ftxui::bgcolor(ftxui::Color::Default));
            } else {
                elements.push_back(ftxui::text(trailStr) | ftxui::color(diamondCol) | ftxui::bgcolor(ftxui::Color::Default));
            }
        }
    }

    return ftxui::hbox(elements);
}

ftxui::Element buildLivePreview(const ConfigState& config) {
    std::vector<PreviewSegment> leftSegments;
    if (config.use_left) {
        for (const auto& block : config.left_order) {
            if (block == "os" && config.show_os)
                leftSegments.push_back({ " OS ", config.os_color });
            else if (block == "session" && config.show_user)
                leftSegments.push_back({ " user@host ", config.user_color });
            else if (block == "path" && config.show_path)
                leftSegments.push_back({ " ~/dir ", config.path_color });
            else if (block == "git" && config.show_git)
                leftSegments.push_back({ " main ", config.git_color });
            else if (block == "time" && config.show_time)
                leftSegments.push_back({ " 12:34 ", config.time_color });
            else if (block == "shell" && config.show_shell)
                leftSegments.push_back({ " bash ", config.shell_color });
            else if (block == "executiontime" && config.show_executiontime)
                leftSegments.push_back({ " 123ms ", config.executiontime_color });
            else if (block == "battery" && config.show_battery)
                leftSegments.push_back({ " 100% ", config.battery_color });
        }
    }

    std::vector<PreviewSegment> rightSegments;
    if (config.use_right) {
        for (const auto& block : config.right_order) {
            if (block == "os" && config.show_os_r)
                rightSegments.push_back({ " OS ", config.os_color_r });
            else if (block == "session" && config.show_user_r)
                rightSegments.push_back({ " user@host ", config.user_color_r });
            else if (block == "path" && config.show_path_r)
                rightSegments.push_back({ " ~/dir ", config.path_color_r });
            else if (block == "git" && config.show_git_r)
                rightSegments.push_back({ " main ", config.git_color_r });
            else if (block == "time" && config.show_time_r)
                rightSegments.push_back({ " 12:34 ", config.time_color_r });
            else if (block == "shell" && config.show_shell_r)
                rightSegments.push_back({ " bash ", config.shell_color_r });
            else if (block == "executiontime" && config.show_executiontime_r)
                rightSegments.push_back({ " 123ms ", config.executiontime_color_r });
            else if (block == "battery" && config.show_battery_r)
                rightSegments.push_back({ " 100% ", config.battery_color_r });
        }
    }

    auto leftEl = renderSegments(leftSegments, config);
    auto rightEl = renderSegments(rightSegments, config);

    return ftxui::hbox({ leftEl, ftxui::filler(), rightEl });
}
