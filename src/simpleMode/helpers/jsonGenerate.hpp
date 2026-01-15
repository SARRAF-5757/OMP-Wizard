#pragma once
#include "../../shared/structs/Config.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

void GenerateJSON(const ConfigState& config);
