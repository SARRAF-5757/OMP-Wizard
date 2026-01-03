#pragma once
#include "ftxui/component/component.hpp"
#include "structs/Config.hpp"

ftxui::Component MakeSimpleMode(ConfigState& state, std::function<void()> on_quit);
