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
	bool show_pl1 = false; //TODO: Change
	bool show_pl2 = false; //TODO: Change
    // diamonds (default is )
    string dmnd_dir_leading = "\uef4d";
    string dmnd_git_leading = "\uef4d";
    string dmnd_git_trailing = "\uef4d";
};

//TODO: Construct a dictionary of diamonds for easier use in the future

int main() {
	//Initialize Stuff
	auto screen = ScreenInteractive::Fullscreen();
	int tabSelected = 0;
	ConfigState config;
	
	std::vector<std::string> tabMessage = {
		"Choose Components to Show in your Prompt",
		"placeholder",
	};
	//Blocks representing data within tabs
    auto tab1Block = Container::Vertical({
		Checkbox("Directory Path", &config.show_dir),
		Checkbox("Git Status", &config.show_git),
		//TODO: add more options
    });
	auto tab2Block = Container::Vertical({
		Checkbox("placeholder1", &config.show_pl1),
		Checkbox("placeholder2", &config.show_pl2),
		//TODO: add more options
    });
	
	//Container of the differing tab blocks
	auto tabContainer = Container::Tab({
		tab1Block,
		tab2Block,
	}, &tabSelected);

	//Container representing the viewed tab
	auto container = Container::Vertical({
		tabContainer,
	});

    //Component to render
    auto toRender = Renderer(container, [&] {
        //Display components vertically
		return vbox({
			text(tabMessage[tabSelected]) | hcenter,
			tabContainer->Render() | hcenter,
			text(" "),
			text(" "),
			hbox(text("Quit [q]"), text("          "), text("Next [n]")) | hcenter,
		});
    });

	//To catch 'q' keypress to exit wizard
    auto component = CatchEvent(toRender, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }
		if (event == Event::Character('n')) {
			if(tabSelected < 1){
				tabSelected = tabSelected + 1;
			}
		}
        return false;
    });


    //Loop and render component on the screen
	screen.Loop(component);
	cout << "Show Directory Path: " << (config.show_dir ? "Yes" : "No") << endl;
	cout << "Show Git Status: " << (config.show_git ? "Yes" : "No") << endl;
	return 0;
}
