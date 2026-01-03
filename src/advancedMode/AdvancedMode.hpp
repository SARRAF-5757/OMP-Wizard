#pragma once
#include "../simpleMode/structs/Config.hpp"
#include "ftxui/component/component.hpp"

ftxui::Component MakeAdvancedMode(ConfigState& state, function<void()> on_quit);
