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

    contVec["tabPickPrompts"] = (tabPickPrompts);
    contVec["tabChooseBlocks"] = (tabChooseBlocks);
    contVec["tabChooseRightBlocks"] = (tabChooseRightBlocks);
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
        if (config.show_os) leftSegments.push_back({ " OS ", config.os_color });
        if (config.show_user) leftSegments.push_back({ " user@host ", config.user_color });
        if (config.show_path) leftSegments.push_back({ " ~/dir ", config.path_color });
        if (config.show_git) leftSegments.push_back({ " main ", config.git_color });
        if (config.show_time) leftSegments.push_back({ " 12:34 ", config.time_color });
        if (config.show_shell) leftSegments.push_back({ " bash ", config.shell_color });
        if (config.show_executiontime) leftSegments.push_back({ " 123ms ", config.executiontime_color });
        if (config.show_battery) leftSegments.push_back({ " 100% ", config.battery_color });
    }

    std::vector<PreviewSegment> rightSegments;
    if (config.use_right) {
        if (config.show_os_r) rightSegments.push_back({ " OS ", config.os_color_r });
        if (config.show_user_r) rightSegments.push_back({ " user@host ", config.user_color_r });
        if (config.show_path_r) rightSegments.push_back({ " ~/dir ", config.path_color_r });
        if (config.show_git_r) rightSegments.push_back({ " main ", config.git_color_r });
        if (config.show_time_r) rightSegments.push_back({ " 12:34 ", config.time_color_r });
        if (config.show_shell_r) rightSegments.push_back({ " bash ", config.shell_color_r });
        if (config.show_executiontime_r) rightSegments.push_back({ " 123ms ", config.executiontime_color_r });
        if (config.show_battery_r) rightSegments.push_back({ " 100% ", config.battery_color_r });
    }

    auto leftEl = renderSegments(leftSegments, config);
    auto rightEl = renderSegments(rightSegments, config);

    return ftxui::hbox({ leftEl, ftxui::filler(), rightEl });
}
