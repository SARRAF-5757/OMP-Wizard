#include <ftxui/component/component_base.hpp>
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

//* Struct to hold different user choices
struct ConfigState {
	//blocks
	bool show_dir = false;
	bool show_git = false;
	//diamonds
	std::string dmnd_dir_leading = " ";
	std::string dmnd_git_leading = " ";
	std::string dmnd_git_trailing = " ";
};

int main() {
	//Initialize Stuff
	auto screen = ftxui::ScreenInteractive::TerminalOutput();
	ConfigState config;

	//Component to render
    auto toRender = ftxui::Renderer([&] {
		//
        ftxui::CheckboxOption opt_path;
        opt_path.label = "directory path";
        opt_path.checked = &config.show_dir;
        ftxui::Component chb_path = ftxui::Checkbox(opt_path);

        ftxui::CheckboxOption opt_git;
        opt_git.label = "git status";
        opt_git.checked = &config.show_git;
        ftxui::Component chb_git = ftxui::Checkbox(opt_git);


        //Display components vertically
		return ftxui::vbox({
			ftxui::text("Choose Components to Show in your Prompt") | ftxui::hcenter,
			chb_path->Render(),
			chb_git->Render()
		});
    });

    //Loop and render component on the screen
	screen.Loop(toRender);
	return 0;
}
