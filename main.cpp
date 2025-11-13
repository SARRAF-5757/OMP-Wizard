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
	bool dmnd_halfC_leading = false;
	bool dmnd_triangle_leading = false;
	bool dmnd_halfC_connect = false;
	bool dmnd_triangle_connect = false;
	bool dmnd_halfC_trail = false;
	bool dmnd_triangle_trail = false;
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
		"Choose Leading Diamond to Show in your Prompt",
	};
	//Blocks representing data within tabs
    auto tabShowStatus = Container::Vertical({
		Checkbox("Directory Path", &config.show_dir),
		Checkbox("Git Status", &config.show_git),
		//TODO: add more options for TAB1
    });
	// TODO: Change to radiobox
	auto tabDmndLead = Container::Vertical({
		Checkbox("\uf139", &config.dmnd_halfC_leading),
		Checkbox("\ueb6f", &config.dmnd_triangle_leading),
    });
	// TODO: Change to radiobox
	std::string dmndConnect = "Choose Connecting Diamond to Show in your Prompt";
	auto tabDmndConnect = Container::Vertical({
		Checkbox("\ue0b4", &config.dmnd_halfC_connect),
		Checkbox("\ueb70", &config.dmnd_triangle_connect),
    });
	// TODO: Change to radiobox
	std::string dmndTrail = "Choose Trailing Diamond to Show in your Prompt";
	auto tabDmndTrail = Container::Vertical({
		Checkbox("\ue0b4", &config.dmnd_halfC_trail),
		Checkbox("\ueb70", &config.dmnd_triangle_trail),
    });
	
	std::vector<Component> showTabs = {
		tabShowStatus,
		tabDmndLead,
	};	

	
	//Container of the differing tab blocks
	auto tabContainer = Container::Tab({
		showTabs,
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
			if(tabSelected < showTabs.size()-1){
				if(tabSelected == 0 && config.show_dir && config.show_git){
					showTabs.push_back(tabDmndConnect);
					tabMessage.push_back(dmndConnect);
					showTabs.push_back(tabDmndTrail);
					tabMessage.push_back(dmndTrail);
				} else if (tabSelected == 0 && !(config.show_dir && config.show_git)){
					showTabs.push_back(tabDmndTrail);
					tabMessage.push_back(dmndTrail);
				}
				tabSelected = tabSelected + 1;
				tabContainer = Container::Tab({
					showTabs,
				}, &tabSelected);
				container->DetachAllChildren();
				container->Add(tabContainer);
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
