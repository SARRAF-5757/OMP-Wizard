#include <fstream>
#include <string>
#include <vector>

#include "../../shared/structs/Config.hpp"
#include "../../shared/structs/Constants.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

//!--------------------------------------Generate JSON-------------------------------------!//
void GenerateJSON(const ConfigState& config) {
    //? Initialize vectors of segment properties and populate them based on selected segments
    std::vector<std::string> types;
    std::vector<std::string> templates;
    std::vector<std::string> colors;
    std::vector<json> segmentsJSON;

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

    //? First, just create all the segments with their basic properties
    // swap where color goes based on color mode
    if (config.color_mode == 0) {
        for (size_t i = 0; i < types.size(); ++i) {
            json individualSegment = {
                {       "type",        types[i] },
                {   "template",    templates[i] },
                {      "style",       "diamond" },
                { "background",       colors[i] },
                { "foreground", config.color_fg }
            };
            segmentsJSON.push_back(individualSegment);
        }
    } else if (config.color_mode == 1) {
        for (size_t i = 0; i < types.size(); ++i) {
            json individualSegment = {
                {       "type",      types[i] },
                {   "template",  templates[i] },
                {      "style",     "diamond" },
                { "background", "transparent" },
                { "foreground",     colors[i] }
            };
            segmentsJSON.push_back(individualSegment);
        }
    } else {
        for (size_t i = 0; i < types.size(); ++i) {
            json individualSegment = {
                {       "type",        types[i] },
                {   "template",    templates[i] },
                {      "style",       "diamond" },
                { "background", config.color_bg },
                { "foreground",       colors[i] }
            };
            segmentsJSON.push_back(individualSegment);
        }
    }

    //? Add diamonds based on position
    //@ If only one block is chosen
    if (types.size() == 1) {
        //@ leading
        if (config.dmnd_leading == 0) {               // No diamond if "None" chosen
            segmentsJSON[0]["leading_diamond"] = "";
        } else if (config.dmnd_leading > 5) {         // offset fix for some leading diamonds
            segmentsJSON[0]["leading_diamond"] = leading_diamonds[config.dmnd_leading] + " ";
        } else {
            segmentsJSON[0]["leading_diamond"] = leading_diamonds[config.dmnd_leading];
        }

        //@ trailing
        if (config.dmnd_trailing== 0) {
          segmentsJSON[0]["trailing_diamond"] = "";
        } else {
          segmentsJSON[0]["trailing_diamond"] = trailing_diamonds[config.dmnd_trailing];
        }
    //@ Multiple blocks chosen
    } else {
        for (size_t i = 0; i < types.size(); i++) {
            //@ for first block (leading + trailing)
            if (i == 0) {
                if (config.dmnd_leading == 0) {
                    segmentsJSON[0]["leading_diamond"] = "";
                } else if (config.dmnd_leading > 5) {
                    segmentsJSON[0]["leading_diamond"] = leading_diamonds[config.dmnd_leading] + " ";
                } else {
                    segmentsJSON[0]["leading_diamond"] = leading_diamonds[config.dmnd_leading];
                }

                if (config.dmnd_connecting == 0) {
                  segmentsJSON[0]["trailing_diamond"] = "";
                } else {
                  segmentsJSON[0]["trailing_diamond"] = trailing_diamonds[config.dmnd_connecting];
                }
            //@ for last block (trailing)
            } else if (i == types.size() - 1) {
                if (config.dmnd_trailing== 0) {
                  segmentsJSON.back()["trailing_diamond"] = "";
                } else {
                  segmentsJSON.back()["trailing_diamond"] = trailing_diamonds[config.dmnd_trailing];
                }
            //@ everything else (connecting)
            } else {
                if (config.dmnd_connecting == 0) {
                  segmentsJSON[i]["trailing_diamond"] = "";
                } else {
                  segmentsJSON[i]["trailing_diamond"] = trailing_diamonds[config.dmnd_connecting];
                }
            }
        }
    }

    //? Special Properties
    for (json& segment : segmentsJSON) {
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

    //? Form our JSON object, starting with just one block that contains an array.
    json j = {
        { "blocks", json::array() }
    };

    // add hardcoded default stuff
    j["$schema"] = "https://raw.githubusercontent.com/JanDeDobbeleer/oh-my-posh/main/themes/schema.json";
    j["version"] = 1;
    j["final_space"] = true;
    j["blocks"].push_back({
      {      "type",     "prompt" },
      { "alignment",       "left" },
      {  "segments", segmentsJSON },
    });

    //? add optional miscellaneous settings
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

    // output JSON into a file with a 4-indent style (basic JSON formatting)
    std::ofstream o("generated-theme.omp.json");
    o << j.dump(4);
    o.close();
}
