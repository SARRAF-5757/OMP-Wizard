#pragma once
#include "../../shared/helpers/colorPicker.hpp"
#include "../../shared/structs/Config.hpp"

//* Content inside each tab
unordered_map<std::string, ftxui::Component> genPageContents(ConfigState& config);

//* Titles for each tab
unordered_map<std::string, std::string> genPageTitles();

//* Function to build the live preview element
ftxui::Element buildLivePreview(const ConfigState& config);
