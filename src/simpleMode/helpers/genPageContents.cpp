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
        return vbox({ text(""), default_checkbox_transform(s), text("") });
    };

    RadioboxOption radiobox_option = RadioboxOption::Simple();
    auto default_radiobox_transform = radiobox_option.transform;
    radiobox_option.transform = [default_radiobox_transform](const EntryState& s) {
        return vbox({ text(""), default_radiobox_transform(s), text("") });
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
    titleVec["leftColorsCombinedTitle"] = "[Left Prompt] Pick Colors for your Selected Blocks";
    titleVec["rightColorsCombinedTitle"] = "[Right Prompt] Pick Colors for your Selected Blocks";

    return titleVec;
}
