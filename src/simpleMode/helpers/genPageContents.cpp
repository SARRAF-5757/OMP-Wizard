#include "../../shared/helpers/colorPicker.hpp"
#include "../../shared/structs/Config.hpp"
#include "../../shared/structs/Constants.hpp"
#include "ftxui/component/component.hpp"


//* Content inside each tab
unordered_map<std::string, ftxui::Component> genPageContents(ConfigState& config) {
    unordered_map<std::string, ftxui::Component> contVec;

    auto tabPickPrompts = Container::Vertical({
      Checkbox("Left Block", &config.use_left),
      Checkbox("Right Block", &config.use_right),
    });
    auto tabChooseBlocks = Container::Vertical({
      Checkbox("User Name", &config.show_user),
      Checkbox("Directory Path", &config.show_path),
      Checkbox("Git Status", &config.show_git),
      Checkbox("Time", &config.show_time),
      Checkbox("Shell Name", &config.show_shell),
    });
    auto tabChooseRightBlocks = Container::Vertical({
      Checkbox("User Name", &config.show_user_r),
      Checkbox("Directory Path", &config.show_path_r),
      Checkbox("Git Status", &config.show_git_r),
      Checkbox("Time", &config.show_time_r),
      Checkbox("Shell Name", &config.show_shell_r),
    });

    contVec["tabPickPrompts"] = (tabPickPrompts);
    contVec["tabChooseBlocks"] = (tabChooseBlocks);
    contVec["tabChooseRightBlocks"] = (tabChooseRightBlocks);
    contVec["tabColorMode"] = Radiobox({ color_mode_choice, &config.color_mode });
    contVec["tabTrPrompt"] = Radiobox({ boolean_choice, &config.tr_prompt });
    contVec["tabTitle"] = Radiobox({ title_choices, &config.title_mode });
    contVec["tabDmndLeading"] = Radiobox({ leading_diamonds, &config.dmnd_leading });
    contVec["tabDmndConnecting"] = Radiobox({ trailing_diamonds, &config.dmnd_connecting });
    contVec["tabDmndTrailing"] = Radiobox({ trailing_diamonds, &config.dmnd_trailing });
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

    return contVec;
}

//* Titles for each tab
unordered_map<std::string, std::string> genPageTitles() {
    unordered_map<std::string, std::string> titleVec;

    titleVec["promptPickTitle"] = "Which prompt sides do you want to configure?";
    titleVec["blocksTitle"] = "Choose Components for the Left Prompt";
    titleVec["rightBlocksTitle"] = "Choose Components for the Right Prompt";
    titleVec["colorModeTitle"] = "How should the prompt(s) be colorized?";
    titleVec["trPromptTitle"] = "Transient Prompt: Do you want to reduce clutter by shrinking your prompt after you hit Enter?";
    titleVec["titleTitle"] = "What should the terminal tab titles look like?";
    titleVec["dmndLeadTitle"] = "Choose Leading Diamond to Show in your Prompt";
    titleVec["dmndConnectTitle"] = "Choose Connecting Diamond to Show in your Prompt";
    titleVec["dmndTrailTitle"] = "Choose Trailing Diamond to Show in your Prompt";
    titleVec["fgTitle"] = "[Both Prompts] Pick a text color";
    titleVec["bgTitle"] = "[Both Prompts] Pick a background color";
    titleVec["userColorTitle"] = "[Left Prompt] Pick a color for your user block";
    titleVec["dirColorTitle"] = "[Left Prompt] Pick a color for your directory block";
    titleVec["gitColorTitle"] = "[Left Prompt] Pick a color for your git block";
    titleVec["timeColorTitle"] = "[Left Prompt] Pick a color for your time block";
    titleVec["shellColorTitle"] = "[Left Prompt] Pick a color for your shell block";
    titleVec["userColorRightTitle"] = "[Right Prompt] Pick a color for your user block";
    titleVec["dirColorRightTitle"] = "[Right Prompt] Pick a color for your directory block";
    titleVec["gitColorRightTitle"] = "[Right Prompt] Pick a color for your git block";
    titleVec["timeColorRightTitle"] = "[Right Prompt] Pick a color for your time block";
    titleVec["shellColorRightTitle"] = "[Right Prompt] Pick a color for your shell block";

    return titleVec;
}
