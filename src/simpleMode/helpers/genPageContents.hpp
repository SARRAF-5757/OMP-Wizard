#include "../helpers/colorPicker.hpp"
#include "../structs/Config.hpp"
#include "../structs/Constants.hpp"
#include "ftxui/component/component.hpp"


//* Content inside each tab
unordered_map<std::string, ftxui::Component> genPageContents(ConfigState& config) {
    unordered_map<std::string, ftxui::Component> contVec;

    auto tabChooseBlocks = Container::Vertical({
      Checkbox("Show User", &config.show_user),
      Checkbox("Directory Path", &config.show_path),
      Checkbox("Git Status", &config.show_git),
    });
    auto tabColorMode = Radiobox({ color_mode_choice, &config.color_mode });
    auto tabTrPrompt = Radiobox({ boolean_choice, &config.tr_prompt });
    auto tabTitle = Radiobox({ title_choices, &config.title_mode });

    // # Block slection based pages
    auto tabDmndLeading = Radiobox({ leading_diamonds, &config.dmnd_leading });
    auto tabDmndConnecting = Radiobox({ trailing_diamonds, &config.dmnd_connecting });
    auto tabDmndTrailing = Radiobox({ trailing_diamonds, &config.dmnd_trailing });
    auto tabFg = colorPicker(&config.fg_color, config.color_fg);
    auto tabBg = colorPicker(&config.bg_color, config.color_bg);
    auto tabUserColor = colorPicker(&config.user_color, config.color_user);
    auto tabDirColor = colorPicker(&config.path_color, config.color_path);
    auto tabGitColor = colorPicker(&config.git_color, config.color_git);

    contVec["tabChooseBlocks"] = (tabChooseBlocks);
    contVec["tabColorMode"] = (tabColorMode);
    contVec["tabTrPrompt"] = (tabTrPrompt);
    contVec["tabTitle"] = (tabTitle);
    contVec["tabDmndLeading"] = (tabDmndLeading);
    contVec["tabDmndConnecting"] = (tabDmndConnecting);
    contVec["tabDmndTrailing"] = (tabDmndTrailing);
    contVec["tabFg"] = (tabFg);
    contVec["tabBg"] = (tabBg);
    contVec["tabUserColor"] = (tabUserColor);
    contVec["tabDirColor"] = (tabDirColor);
    contVec["tabGitColor"] = (tabGitColor);

    return contVec;
}

//* Titles for each tab
unordered_map<std::string, std::string> genPageTitles() {
    unordered_map<std::string, std::string> titleVec;

    std::string blocksTitle = "Choose Components to Show in your Prompt";
    std::string colorModeTitle = "How should the prompt be colorized?";
    std::string trPromptTitle = "Transient Prompt: Do you want to reduce clutter by shrinking your prompt after you hit Enter?";
    std::string titleTitle = "What should the terminal tab titles look like?";

    // # Block slection based pages
    std::string dmndLeadTitle = "Choose Leading Diamond to Show in your Prompt";
    std::string dmndConnectTitle = "Choose Connecting Diamond to Show in your Prompt";
    std::string dmndTrailTitle = "Choose Trailing Diamond to Show in your Prompt";
    std::string fgTitle = "Pick a text color";
    std::string bgTitle = "Pick a background color";
    std::string userColorTitle = "Pick a color for your user block";
    std::string dirColorTitle = "Pick a color for your directory block";
    std::string gitColorTitle = "Pick a color for your git block";

    titleVec["blocksTitle"] = (blocksTitle);
    titleVec["colorModeTitle"] = (colorModeTitle);
    titleVec["trPromptTitle"] = (trPromptTitle);
    titleVec["titleTitle"] = (titleTitle);
    titleVec["dmndLeadTitle"] = (dmndLeadTitle);
    titleVec["dmndConnectTitle"] = (dmndConnectTitle);
    titleVec["dmndTrailTitle"] = (dmndTrailTitle);
    titleVec["fgTitle"] = (fgTitle);
    titleVec["bgTitle"] = (bgTitle);
    titleVec["userColorTitle"] = (userColorTitle);
    titleVec["dirColorTitle"] = (dirColorTitle);
    titleVec["gitColorTitle"] = (gitColorTitle);

    return titleVec;
}
