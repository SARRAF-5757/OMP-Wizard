#include <fstream>
#include <string>
#include <vector>

#include "../../shared/helpers/deploy.hpp"
#include "../../shared/structs/Config.hpp"
#include "../../shared/structs/Constants.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

//!--------------------------------------Generate JSON-------------------------------------!//

// # Helper: add special properties (path style, git options) to a list of segments
static void addSpecialProperties(std::vector<json>& segments) {
    for (json& segment : segments) {
        if (segment["type"] == "path") {
            segment["options"] = {
                { "style", "folder" }
            };
        }
        if (segment["type"] == "git") {
            segment["options"] = {
                {         "branch_icon", " \ue725 " },
                {        "fetch_status",       true },
                { "fetch_upstream_icon",       true }
            };
        }
    }
}

// # Helper: build segment JSON objects from types/templates/colors using the given color mode and style
static std::vector<json> buildSegments(const std::vector<std::string>& types, const std::vector<std::string>& templates,
                                       const std::vector<std::string>& colors, int color_mode, const std::string& color_fg,
                                       const std::string& color_bg, const std::string& style) {
    std::vector<json> segmentsJSON;
    for (size_t i = 0; i < types.size(); ++i) {
        json seg;
        seg["type"] = types[i];
        seg["template"] = templates[i];
        seg["style"] = style;

        if (color_mode == 0) {   // colored background mode
            seg["background"] = colors[i];
            seg["foreground"] = color_fg;
        } else if (color_mode == 1) {   // transparent background mode
            seg["background"] = "transparent";
            seg["foreground"] = colors[i];
        } else {   // monochrome background mode
            seg["background"] = color_bg;
            seg["foreground"] = colors[i];
        }
        segmentsJSON.push_back(seg);
    }
    return segmentsJSON;
}


// # Helper: build a complete prompt block (left or right)
static json buildPromptBlock(const std::string& alignment, const std::vector<std::string>& types, const std::vector<std::string>& templates,
                             const std::vector<std::string>& colors, const ConfigState& config) {
    std::vector<json> segmentsJSON = buildSegments(types, templates, colors, config.color_mode, config.color_fg, config.color_bg, "diamond");

    if (!segmentsJSON.empty()) {
        if (segmentsJSON.size() == 1) {
            segmentsJSON[0]["leading_diamond"] = (config.dmnd_leading == 0) ? "" : leading_diamonds[config.dmnd_leading];
            segmentsJSON[0]["trailing_diamond"] = (config.dmnd_trailing == 0) ? "" : trailing_diamonds[config.dmnd_trailing];
        } else {
            for (size_t i = 0; i < segmentsJSON.size(); i++) {
                if (i == 0) {
                    segmentsJSON[0]["leading_diamond"] = (config.dmnd_leading == 0) ? "" : leading_diamonds[config.dmnd_leading];
                    segmentsJSON[0]["trailing_diamond"] = (config.dmnd_connecting == 0) ? "" : trailing_diamonds[config.dmnd_connecting];
                } else if (i == segmentsJSON.size() - 1) {
                    segmentsJSON.back()["trailing_diamond"] = (config.dmnd_trailing == 0) ? "" : trailing_diamonds[config.dmnd_trailing];
                } else {
                    segmentsJSON[i]["trailing_diamond"] = (config.dmnd_connecting == 0) ? "" : trailing_diamonds[config.dmnd_connecting];
                }
            }
        }
    }

    addSpecialProperties(segmentsJSON);

    return {
        {      "type",     "prompt" },
        { "alignment",    alignment },
        {  "segments", segmentsJSON }
    };
}


void GenerateJSON(const ConfigState& config) {
    //? Form our JSON object
    json j = {
        { "blocks", json::array() }
    };
    j["$schema"] = "https://raw.githubusercontent.com/JanDeDobbeleer/oh-my-posh/main/themes/schema.json";
    j["version"] = 1;
    j["final_space"] = true;


    //? ======================== LEFT PROMPT ========================
    if (config.use_left) {
        std::vector<std::string> types, templates, colors;

        if (config.show_os) {
            types.emplace_back("os");
            templates.emplace_back(" {{ .Icon }} ");
            colors.push_back(config.color_os);
        }
        if (config.show_user) {
            types.emplace_back("session");
            templates.emplace_back(" {{ .UserName }} ");
            colors.push_back(config.color_user);
        }
        if (config.show_path) {
            types.emplace_back("path");
            templates.emplace_back(" \ue5ff {{ .Path }} ");
            colors.push_back(config.color_path);
        }
        if (config.show_git) {
            types.emplace_back("git");
            templates.emplace_back(
              " {{ .UpstreamIcon }}{{ .HEAD }}{{if .BranchStatus }} {{ .BranchStatus }}{{ end }}{{ if .Working.Changed }} "
              "\uf044 {{ .Working.String }}{{ end }}{{ if and (.Working.Changed) (.Staging.Changed) }} |{{ end }}{{ if "
              ".Staging.Changed }} \uf046 {{ .Staging.String }}{{ end }}{{ if gt .StashCount 0 }} \ueb4b {{ .StashCount "
              "}}{{ end }} ");
            colors.push_back(config.color_git);
        }
        if (config.show_time) {
            types.emplace_back("time");
            templates.emplace_back("\ue641 {{ .CurrentDate | date .Format }}");
            colors.push_back(config.color_time);
        }
        if (config.show_shell) {
            types.emplace_back("shell");
            templates.emplace_back("\uf489 {{ .Name }}");
            colors.push_back(config.color_shell);
        }
        if (config.show_executiontime) {
            types.emplace_back("executiontime");
            templates.emplace_back("\ufa1e {{ .FormattedMs }} ");
            colors.push_back(config.color_executiontime);
        }
        if (config.show_battery) {
            types.emplace_back("battery");
            templates.emplace_back(" {{ if not .Error }}{{ .Icon }}{{ .Percentage }}{{ end }}% ");
            colors.push_back(config.color_battery);
        }

        j["blocks"].push_back(buildPromptBlock("left", types, templates, colors, config));
    }


    //? ======================== RIGHT PROMPT ========================
    if (config.use_right) {
        std::vector<std::string> rightTypes, rightTemplates, rightColors;

        if (config.show_os_r) {
            rightTypes.emplace_back("os");
            rightTemplates.emplace_back(" {{ .Icon }} ");
            rightColors.push_back(config.color_os_r);
        }
        if (config.show_user_r) {
            rightTypes.emplace_back("session");
            rightTemplates.emplace_back(" {{ .UserName }} ");
            rightColors.push_back(config.color_user_r);
        }
        if (config.show_path_r) {
            rightTypes.emplace_back("path");
            rightTemplates.emplace_back(" \ue5ff {{ .Path }} ");
            rightColors.push_back(config.color_path_r);
        }
        if (config.show_git_r) {
            rightTypes.emplace_back("git");
            rightTemplates.emplace_back(
              " {{ .UpstreamIcon }}{{ .HEAD }}{{if .BranchStatus }} {{ .BranchStatus }}{{ end }}{{ if .Working.Changed }} "
              "\uf044 {{ .Working.String }}{{ end }}{{ if and (.Working.Changed) (.Staging.Changed) }} |{{ end }}{{ if "
              ".Staging.Changed }} \uf046 {{ .Staging.String }}{{ end }}{{ if gt .StashCount 0 }} \ueb4b {{ .StashCount "
              "}}{{ end }} ");
            rightColors.push_back(config.color_git_r);
        }
        if (config.show_time_r) {
            rightTypes.emplace_back("time");
            rightTemplates.emplace_back("\ue641 {{ .CurrentDate | date .Format }}");
            rightColors.push_back(config.color_time_r);
        }
        if (config.show_shell_r) {
            rightTypes.emplace_back("shell");
            rightTemplates.emplace_back("\uf489 {{ .Name }}");
            rightColors.push_back(config.color_shell_r);
        }
        if (config.show_executiontime_r) {
            rightTypes.emplace_back("executiontime");
            rightTemplates.emplace_back("\ufa1e {{ .FormattedMs }} ");
            rightColors.push_back(config.color_executiontime_r);
        }
        if (config.show_battery_r) {
            rightTypes.emplace_back("battery");
            rightTemplates.emplace_back(" {{ if not .Error }}{{ .Icon }}{{ .Percentage }}{{ end }}% ");
            rightColors.push_back(config.color_battery_r);
        }

        j["blocks"].push_back(buildPromptBlock("right", rightTypes, rightTemplates, rightColors, config));

        //* add newline / static left block if right prompt is added
        {
            json staticSegment = {
                {   "template",                               "\ue285\ue285" },
                { "foreground",                            config.color_user },
                {       "type",                                     "status" },
                {      "style",                                      "plain" },
                { "properties", json::object({ { "always_enabled", true } }) },
            };

            j["blocks"].push_back({
              {      "type",                       "prompt" },
              { "alignment",                         "left" },
              {  "segments", json::array({ staticSegment }) },
              {   "newline",                           true },
            });
        }
    }


    // # add optional miscellaneous settings
    if (!config.tr_prompt) {
        j["transient_prompt"] = {
            {   "template",     "\ue285 " },
            { "foreground",     "#AB76D9" },
            { "background", "transparent" }
        };
    }

    if (config.title_mode == 1) {
        j["console_title_template"] = "{{ .Folder }}";
    } else if (config.title_mode == 2) {
        j["console_title_template"] = "{{ base (dir .PWD)}}/{{ .Folder }}";
    } else if (config.title_mode == 3) {
        j["console_title_template"] = "{{ .PWD }}";
    } else if (config.title_mode == 4) {
        j["console_title_template"] = "{{ .Shell }} in {{ .PWD }}";
    }

    // send the JSON to the deployment function, with a 4-indent style (basic JSON formatting)
    deployConfig(j.dump(4));
}
