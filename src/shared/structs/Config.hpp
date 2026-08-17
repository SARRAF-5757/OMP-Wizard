#pragma once
#include <string>
#include <vector>

#include "../../shared/structs/RGB.hpp"
#include "ftxui/component/screen_interactive.hpp"


//* Struct to hold different user choices
struct ConfigState {
    // flags from main
    int tabSelected = 0;
    bool tabsAdded = false;

    bool use_left = false;
    bool use_right = false;
    bool use_newlineBlock = false;

    // vectors for left tabs (in main)
    std::vector<ftxui::Component> showTabs;
    std::vector<std::string> tabMessage;
    // vectors for right tabs (in main)
    std::vector<ftxui::Component> showRightTabs;
    std::vector<std::string> tabRightMessage;

    // left prompt block choices
    bool show_user = false;
    bool show_path = false;
    bool show_git = false;
    bool show_time = false;
    bool show_shell = false;
    bool show_executiontime = false;
    bool show_battery = false;
    bool show_os = false;
    // right prompt block choices
    bool show_user_r = false;
    bool show_path_r = false;
    bool show_git_r = false;
    bool show_time_r = false;
    bool show_shell_r = false;
    bool show_executiontime_r = false;
    bool show_battery_r = false;
    bool show_os_r = false;

    // general block colors
    std::string color_fg = "#FFFFFF";
    std::string color_bg = "transparent";
    // left prompt block colors
    std::string color_user = "#FFFFFF";
    std::string color_path = "#FFFFFF";
    std::string color_git = "#FFFFFF";
    std::string color_time = "#FFFFFF";
    std::string color_shell = "#FFFFFF";
    std::string color_executiontime = "#FFFFFF";
    std::string color_battery = "#FFFFFF";
    std::string color_os = "#FFFFFF";
    // right prompt block colors
    std::string color_user_r = "#FFFFFF";
    std::string color_path_r = "#FFFFFF";
    std::string color_git_r = "#FFFFFF";
    std::string color_time_r = "#FFFFFF";
    std::string color_shell_r = "#FFFFFF";
    std::string color_executiontime_r = "#FFFFFF";
    std::string color_battery_r = "#FFFFFF";
    std::string color_os_r = "#FFFFFF";

    // diamond choices
    int dmnd_leading = 0;
    int dmnd_connecting = 0;
    int dmnd_trailing = 0;
    // misc options
    int tr_prompt = 0;
    int color_mode = 0;
    int title_mode = 0;

    // setting default colors on pages
    rgb fg_color = { 0, 0, 0 };
    rgb bg_color = { 18, 19, 20 };
    // block ordering
    std::vector<std::string> left_order = {"os", "session", "path", "git", "time", "shell", "executiontime", "battery"};
    std::vector<std::string> right_order = {"os", "session", "path", "git", "time", "shell", "executiontime", "battery"};
    // left prompt colors
    rgb os_color = { 107, 108, 255 };
    rgb user_color = { 171, 118, 217 };
    rgb path_color = { 130, 170, 255 };
    rgb git_color = { 255, 223, 109 };
    rgb time_color = { 191, 204, 229 };
    rgb shell_color = { 140, 223, 109 };
    rgb executiontime_color = { 255, 255, 100 };
    rgb battery_color = { 255, 146, 120 };
    // right prompt colors
    rgb os_color_r = { 107, 108, 255 };
    rgb user_color_r = { 171, 118, 217 };
    rgb path_color_r = { 130, 170, 255 };
    rgb git_color_r = { 255, 223, 109 };
    rgb time_color_r = { 191, 204, 229 };
    rgb shell_color_r = { 140, 223, 109 };
    rgb executiontime_color_r = { 255, 255, 100 };
    rgb battery_color_r = { 255, 146, 120 };

    // advanced mode exclusives
    int template_mode = 0;
    int option_mode = 0;
    int island_mode = 0;
};
