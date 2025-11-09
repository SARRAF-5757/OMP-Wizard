#include <iostream>
#include <ftxui/component/component_base.hpp>
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "nlohmann/json.hpp"

using namespace std;
using namespace ftxui;
using json = nlohmann::json;

//* Struct to hold different user choices
struct ConfigState {
	//blocks
	bool show_dir = false;
	bool show_git = false;
    // diamonds (default is )
    string dmnd_dir_leading = "\uef4d";
    string dmnd_git_leading = "\uef4d";
    string dmnd_git_trailing = "\uef4d";
};

//TODO: Construct a dictionary of diamonds for easier use in the future

int main() {
	//Initialize Stuff
	auto screen = ScreenInteractive::TerminalOutput();
	ConfigState config;

	//Checkboxes to choose which blocks to pick in the prompt
    auto blocks = Container::Vertical({
		Checkbox("Directory Path", &config.show_dir),
		Checkbox("Git Status", &config.show_git),
		//TODO: add more options
    });


    //Component to render
    auto toRender = Renderer(blocks, [&] {
        //Display components vertically
		return vbox({
			text("Choose Components to Show in your Prompt") | hcenter,
			blocks->Render() | hcenter
		});
    });


    //Loop and render component on the screen
	screen.Loop(toRender);
	cout << "Show Directory Path: " << (config.show_dir ? "Yes" : "No") << endl;
	cout << "Show Git Status: " << (config.show_git ? "Yes" : "No") << endl;
	return 0;
}
