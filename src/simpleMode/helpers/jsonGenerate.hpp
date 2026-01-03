#pragma once
#include <fstream>
#include <string>
#include <vector>

#include "../structs/Config.hpp"
#include "../structs/Diamonds.hpp"
#include "nlohmann/json.hpp"


using namespace std;
using json = nlohmann::json;

//!--------------------------------------Generate JSON-------------------------------------!//
void GenerateJSON(const ConfigState& config) {
    //? Initialize vectors of segment properties and populate them based on selected segments
    vector<string> types;
    vector<string> templates;
    vector<string> colors;
    vector<json> segmentsJSON;

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

    //? Now add diamonds based on position
    if (types.size() == 1) {
        // offset fix for some leading diamonds
        if (config.dmnd_leading > 4) {
            segmentsJSON[0]["leading_diamond"] = leading_diamonds[config.dmnd_leading] + " ";
        } else {
            segmentsJSON[0]["leading_diamond"] = leading_diamonds[config.dmnd_leading];
        }
        segmentsJSON[0]["trailing_diamond"] = trailing_diamonds[config.dmnd_trailing];
    } else {
        for (size_t i = 0; i < types.size(); i++) {
            if (i == 0) {
                if (config.dmnd_leading > 4) {
                    segmentsJSON[0]["leading_diamond"] = leading_diamonds[config.dmnd_leading] + " ";
                } else {
                    segmentsJSON[0]["leading_diamond"] = leading_diamonds[config.dmnd_leading];
                }
                segmentsJSON[i]["trailing_diamond"] = trailing_diamonds[config.dmnd_connecting];
            } else if (i == types.size() - 1) {
                segmentsJSON.back()["trailing_diamond"] = trailing_diamonds[config.dmnd_trailing];
            } else {
                segmentsJSON[i]["trailing_diamond"] = trailing_diamonds[config.dmnd_connecting];
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
    ofstream o("generated-theme.omp.json");
    o << j.dump(4);
    o.close();
}
