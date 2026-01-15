#pragma once
#include <string>
#include <vector>

//* List of leading diamond choices (radiobox entries)
const std::vector<std::string> leading_diamonds = {
    "None",                 // straight edge
    "\u2591\u2592\u2593",   // left fading blur
    "\ue0b6",               // left half-circle
    "\ue0b2",               // left triangle
    "\ue0ba",               // left lower slant
    "\ue0be",               // left upper slant
    "\ue0c2",               // left fire
    "\ue0ca",               // left shockwave
    "\ue0c7",               // left big square fade
    "\ue0c5",               // left small square fade
    "Custom",               // input hex code
};

//* List of trailing diamond choices (radiobox entries)
const std::vector<std::string> trailing_diamonds = {
    "None",                 // straight edge
    "\u2593\u2592\u2591",   // right fading blur
    "\ue0b4",               // right half-circle
    "\ue0b0",               // right triangle
    "\ue0bc",               // right upper slant
    "\ue0b8",               // right lower slant
    "\ue0c0",               // right fire
    "\ue0c8",               // right shockwave
    "\ue0c6",               // right big square fade
    "\ue0c4",               // right small square fade
    "Custom",               // input hex code
};

//* For miscellaneous options
const std::vector<std::string> boolean_choice = { "Yes", "No" };
const std::vector<std::string> color_mode_choice
  = { "Colored Background", "Colored Text (Transparent Background)", "Colored Text (Monochrome Background)" };
const std::vector<std::string> title_choices = { "Default", "Current folder", "Parent folder/current folder", "Full Path", "'Shell' in 'Full Path'" };


//* Advanced Specific
const std::vector<std::string> adv_color_mode_choice = { "Transparent Background [No Diamonds]", "Colored Background" };
const std::vector<std::string> template_mode_choice = { "Default Template", "Custom" };
const std::vector<std::string> options_mode_choice = { "Default Options", "Custom" };
const std::vector<std::string> island_mode_choice = { "No", "Yes" };
