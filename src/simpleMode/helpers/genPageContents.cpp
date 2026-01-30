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
      Checkbox("Show User", &config.show_user),
      Checkbox("Directory Path", &config.show_path),
      Checkbox("Git Status", &config.show_git),
    });

    contVec["tabPickPrompts"] = (tabPickPrompts);
    contVec["tabChooseBlocks"] = (tabChooseBlocks);
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

    return contVec;
}

//* Titles for each tab
unordered_map<std::string, std::string> genPageTitles() {
    unordered_map<std::string, std::string> titleVec;

    titleVec["blocksTitle"] = "Choose Components to Show in your Prompt";
    titleVec["colorModeTitle"] = "How should the prompt be colorized?";
    titleVec["trPromptTitle"] = "Transient Prompt: Do you want to reduce clutter by shrinking your prompt after you hit Enter?";
    titleVec["titleTitle"] = "What should the terminal tab titles look like?";
    titleVec["dmndLeadTitle"] = "Choose Leading Diamond to Show in your Prompt";
    titleVec["dmndConnectTitle"] = "Choose Connecting Diamond to Show in your Prompt";
    titleVec["dmndTrailTitle"] = "Choose Trailing Diamond to Show in your Prompt";
    titleVec["fgTitle"] = "Pick a text color";
    titleVec["bgTitle"] = "Pick a background color";
    titleVec["userColorTitle"] = "Pick a color for your user block";
    titleVec["dirColorTitle"] = "Pick a color for your directory block";
    titleVec["gitColorTitle"] = "Pick a color for your git block";

    return titleVec;
}
