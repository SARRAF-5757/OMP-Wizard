#pragma once
#include <string>
#include <vector>

#include "Colors.hpp"
#include "ftxui/component/screen_interactive.hpp"

using namespace std;
using namespace ftxui;

//* Struct to hold different user choices
struct ConfigState {
    // flags from main
    int tabSelected = 0;
    bool tabsAdded = false;

    // vectors for tabs (in main)
    vector<Component> showTabs;
    vector<string> tabMessage;

    // block choices
    bool show_user = false;
    bool show_path = false;
    bool show_git = false;
    // block colors
    string color_fg = "transparent";
    string color_bg = "#000000";
    string color_user = "#FFFFFF";
    string color_path = "#FFFFFF";
    string color_git = "#FFFFFF";
    // diamond choices
    int dmnd_leading = 0;
    int dmnd_connecting = 0;
    int dmnd_trailing = 0;
    // misc options
    int tr_prompt = 0;
    int color_mode = 0;
    int title_mode = 0;

    // colors
    rgb fg_color = { 0, 0, 0 };
    rgb bg_color = { 18, 19, 20 };
    rgb user_color = { 171, 118, 217 };
    rgb path_color = { 130, 170, 255 };
    rgb git_color = { 255, 223, 109 };
};
