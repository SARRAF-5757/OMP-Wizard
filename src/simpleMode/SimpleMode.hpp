#pragma once
#include "../shared/structs/Config.hpp"
#include "ftxui/component/component.hpp"

ftxui::Component MakeSimpleMode(ConfigState& state, std::function<void()> on_quit);
