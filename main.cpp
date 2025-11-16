#include <iostream>
#include <ftxui/component/component_base.hpp>
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "nlohmann/json.hpp"

using namespace std;
using namespace ftxui;
using json = nlohmann::json;

//^ Struct to hold different user choices
struct ConfigState {
	//block choices
	bool show_dir = false;
	bool show_git = false;
    // diamond choices
    int dmnd_leading = 0;
    int dmnd_connecting = 0;
    int dmnd_trailing = 0;
};

// TODO: Construct a dictionary of diamonds for human-readable usage in the future

//^ List of leading diamond choices (radiobox entries)
vector<string> leading_diamonds = {
    "\ue0b6",	//left half-circle
    "\ue0b0",	//left triangle
    "\uf0c8",	//square
	"\ue0ba",	//left lower slant
	"\ue0be",	//left upper slant
	"\ue0c2",	//left fire
	"\ue0ca",	//left shockwave
	"\ue0c7",	//left big square fade
	"\ue0c5",	//left small square fade
};

//^ List of trailing diamond choices (radiobox entries)
vector<string> trailing_diamonds = {
    "\ue0b4",	//right half-circle
    "\ue0b2",	//right triangle
    "\uf0c8",	//square
	"\ue0bc",	//right upper slant
	"\ue0b8",	//right lower slant
	"\ue0c0",	//right fire
	"\ue0c8",	//right shockwave
	"\e0c6",	//right big square fade
	"\ue0c4",	//right small square fade
};

int main() {
	//Initialize Stuff
	auto screen = ScreenInteractive::Fullscreen();
	int tabSelected = 0;
	ConfigState config;


	//* Content inside each tab
    auto tabChooseBlocks = Container::Vertical({
		Checkbox("Directory Path", &config.show_dir),
		Checkbox("Git Status", &config.show_git),
		//TODO: add more block choices
    });

	auto tabDmndLeading = Radiobox({
		.entries = leading_diamonds,
		.selected = &config.dmnd_leading,
    });
	
	string dmndConnectTitle = "Choose Connecting Diamond to Show in your Prompt";
    auto tabDmndConnecting = Radiobox({
		.entries = trailing_diamonds,
		.selected = &config.dmnd_connecting,
    });

	string dmndTrailTitle = "Choose Trailing Diamond to Show in your Prompt";
    auto tabDmndTrailing = Radiobox({
		.entries = trailing_diamonds,
		.selected = &config.dmnd_trailing,
    });


	//^ Vectors to switch contents among tabs (preloaded with first two screens)
	//Vector of components representing each tab
    vector<Component> showTabs = {
		tabChooseBlocks,
		tabDmndLeading
	};
	//Titles shown on each screens
	vector<string> tabMessage = {
		"Choose Components to Show in your Prompt",
		"Choose Leading Diamond to Show in your Prompt"
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
			text(" "),
			tabContainer->Render() | hcenter,
			text(" "),
			text(" "),
			vbox({
				text("[q] = Quit the wizard without generating config file"),
				text("[n] = Confirm selections & go to the next screen"),
				text("[e] = End the wizard and generate config file"),
				text("      (will use defaults if all options are not specified)")
			}) | hcenter
		}) | vcenter;
    });

	//To catch 'q' keypress to exit wizard
    auto component = CatchEvent(toRender, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }
		if (event == Event::Character('n')) {
			if(tabSelected < showTabs.size()-1){
				//if only one block is selected, only offer leading and trailing diamond
				if(tabSelected == 0 && (config.show_dir ^ config.show_git)){
                    showTabs.push_back(tabDmndTrailing);
                    tabMessage.push_back(dmndTrailTitle);
                } else if (tabSelected == 0) {
				//if multiple blocks are choosen, show all diamond choices
					showTabs.push_back(tabDmndConnecting);
                    tabMessage.push_back(dmndConnectTitle);
					
                    showTabs.push_back(tabDmndTrailing);
                    tabMessage.push_back(dmndTrailTitle);
                }
				
				//stop advancing if no blocks are chosen or if at the last page
				if ((config.show_dir || config.show_git) && (tabSelected < showTabs.size()-1)) {
                    tabSelected = tabSelected + 1;
					tabContainer = Container::Tab({
						showTabs,
					}, &tabSelected);
					container->DetachAllChildren();
					container->Add(tabContainer);
                }
			}
		}
		if (event == Event::Character('e')) {
			//TODO: End wizard and generate Json
            screen.ExitLoopClosure()();	//temp, does the same as quitting
            return true;
        }
        return false;
    });


    //Loop and render component on the screen
	screen.Loop(component);
	cout << "Show Directory Path: " << (config.show_dir ? "Yes" : "No") << endl;
	cout << "Show Git Status: " << (config.show_git ? "Yes" : "No") << endl;

	// initialize vector of selected segments, and then formatted JSON segments
	std::vector<std::string> segments;
	std::vector<json> segmentsJSON;
	if(config.show_git){
		segments.push_back("git");
	}

	if(config.show_dir){
		segments.push_back("path");
	}

	// format JSON Segments based on selected segments
	if(segments.size() == 1){
		json individualSegment = {
				{"type", segments[0]},
				{"leading_diamond", leading_diamonds[config.dmnd_leading]},
				{"trailing_diamond", trailing_diamonds[config.dmnd_trailing]}
			};
		segmentsJSON.push_back(individualSegment);
	} else {
		for(size_t i = 0; i < segments.size(); i++){
			// first iteration 
			json individualSegment;
			if (i == 0) {
				individualSegment = {					
					{"type", segments[i]},
					{"leading_diamond", leading_diamonds[config.dmnd_leading]},
					{"trailing_diamond", trailing_diamonds[config.dmnd_connecting]}
				};
			} else {
				individualSegment = {
					{"type", segments[i]},
					{"trailing_diamond", trailing_diamonds[config.dmnd_trailing]}
				};
			}
			segmentsJSON.push_back(individualSegment);
		}
	}
	// form our JSON object, starting with just one block that contains an array.
	json j = {
    { "blocks", json::array() }
	};

	// push these values to the first block
	j["blocks"].push_back({
	{ "type", "prompt" },
	{ "alignment", "left" },
    { "segments", segmentsJSON }
	});
	
	// print out the JSON file with a 4-indent style (basic JSON formatting)
	cout << j.dump(4);

	return 0;
}
