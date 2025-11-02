#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include "ftxui/component/component.hpp"            // For Components
#include "ftxui/component/screen_interactive.hpp"   // For ScreenInteractive
#include "ftxui/dom/elements.hpp"                   // For DOM elements (text, border, etc)
#include "nlohmann/json.hpp"   						// For JSON creation

using namespace ftxui;
using json = nlohmann::json;


//* States for which question we're on
enum class QuestionState { 	ShowPath,
						PathLeadingDiamond,
						ShowGit,
						GitConnectingDiamond,
						GitTrailingDiamond,
						Done
					};

//* Struct to hold answers from the user
struct ConfigState {
    bool show_path = false;
    std::string path_leading_diamond = " \uE0B0";   // Default: 
    bool show_git = false;
    std::string git_connecting_diamond = " \uE0B0";
    std::string git_trailing_diamond = " \uE0B0";
};

//^ Function to generate the JSON config file
void GenerateJsonConfig(const ConfigState& state) {
    json config;
    json blocks = json::array();
    json main_block;
    json segments = json::array();

    main_block["type"] = "prompt";
    main_block["alignment"] = "left";

    // --- 1. Add Path Segment ---
    if (state.show_path) {
        json path_segment;
        path_segment["type"] = "path";
        path_segment["style"] = "diamond";
        path_segment["leading_diamond"] = state.path_leading_diamond;
        path_segment["properties"]["style"] = "full";   // Show full path
        segments.push_back(path_segment);
    }

    // --- 2. Add Git Segment ---
    if (state.show_git) {
        json git_segment;
        git_segment["type"] = "git";
        git_segment["style"] = "diamond";

        // If we have a path segment, this is the *connecting* diamond
        if (state.show_path) {
            segments.back()["trailing_diamond"] = state.git_connecting_diamond;
        } else {
            // No path, so this is the *leading* diamond for the git segment
            git_segment["leading_diamond"] = state.git_connecting_diamond;
        }

        git_segment["properties"]["fetch_status"] = true;
        segments.push_back(git_segment);
    }

    // --- 3. Add Final Trailing Diamond ---
    if (!segments.empty()) {
        segments.back()["trailing_diamond"] = state.git_trailing_diamond;
    }

    // --- Assemble and Write File ---
    main_block["segments"] = segments;
    blocks.push_back(main_block);

    // Add a final "exit" segment to make the prompt look clean
    json exit_segment;
    exit_segment["type"] = "text";
    exit_segment["style"] = "transparent";
    exit_segment["properties"]["text"] = " ";   // Just a space
    segments.push_back(exit_segment);

    main_block["segments"] = segments;
    config["blocks"] = blocks;
    config["version"] = 2;   // OMP v2 schema

    // Write to file
    std::ofstream o("my_theme.omp.json");
    o << std::setw(4) << config << std::endl;
    o.close();
}


//! Main function
int main(int argc, const char* argv[]) {
    auto screen = ScreenInteractive::TerminalOutput();

    QuestionState qState = QuestionState::ShowPath;
    ConfigState config;

    int selected_yes_no = 0;
    std::vector<std::string> yes_no_options = { "Yes", "No" };

    std::string path_diamond_input = " \uE0B0";
    std::string git_connect_diamond_input = " \uE0B0";
    std::string git_trail_diamond_input = " \uE0B0";

    // --- Main Render Loop (runs on enter key) ---
    std::function<void()> on_enter;
    auto renderer = Renderer([&] {
        Component current_question_component;
        std::string question_text;

        switch (qState) {
        case QuestionState::ShowPath:
            question_text = "Do you want the current directory in your prompt?";
            current_question_component = Menu(&yes_no_options, &selected_yes_no);
            on_enter = [&] {
                config.show_path = (selected_yes_no == 0);
                qState = config.show_path ? QuestionState::PathLeadingDiamond : QuestionState::ShowGit;
                selected_yes_no = 0;   // Reset for next question
            };
            break;

        case QuestionState::PathLeadingDiamond:
            question_text = "Enter the leading diamond/separator for the path:";
            current_question_component = Input(&path_diamond_input, "separator");
            on_enter = [&] {
                config.path_leading_diamond = path_diamond_input;
                qState = QuestionState::ShowGit;
            };
            break;

        case QuestionState::ShowGit:
            question_text = "Do you want git status?";
            current_question_component = Menu(&yes_no_options, &selected_yes_no);
            on_enter = [&] {
                config.show_git = (selected_yes_no == 0);
                if (config.show_git) {
                    // Ask for both connecting and ending diamonds
                    qState = QuestionState::GitConnectingDiamond;
                } else {
                    // No git, just ask for the final ending diamond
                    qState = QuestionState::GitTrailingDiamond;
                }
                selected_yes_no = 0;   // Reset
            };
            break;

        case QuestionState::GitConnectingDiamond:
            question_text = "Enter the connecting diamond (between path and git):";
            if (!config.show_path) {
                question_text = "Enter the leading diamond for the git segment:";
            }
            current_question_component = Input(&git_connect_diamond_input, "separator");
            on_enter = [&] {
                config.git_connecting_diamond = git_connect_diamond_input;
                qState = QuestionState::GitTrailingDiamond;
            };
            break;

        case QuestionState::GitTrailingDiamond:
            question_text = "Enter the final ending diamond:";
            current_question_component = Input(&git_trail_diamond_input, "separator");
            on_enter = [&] {
                config.git_trailing_diamond = git_trail_diamond_input;
                qState = QuestionState::Done;
            };
            break;

        case QuestionState::Done:
            GenerateJsonConfig(config);
            screen.Exit();   // Exit the application
            return text("Done! Config saved to my_theme.omp.json");
        }

        // --- Render the UI ---
        // We catch the "Enter" key to submit the current question.
        auto component_with_handler = CatchEvent(current_question_component, [&](Event event) {
            if (event == Event::Return) {
                on_enter();
                return true;   // Event was handled
            }
            return false;   // Event was not handled
        });

        return vbox({ text(question_text) | bold | hcenter, separator(),
			component_with_handler->Render() | center,
			separator(), text("Press 'Enter' to confirm. Use 'Tab' or Arrows to navigate.") | dim })
			| border | center;
    });

    screen.Loop(renderer);

    std::cout << "Configuration file 'my_theme.omp.json' generated successfully!" << std::endl;
    return 0;
}
