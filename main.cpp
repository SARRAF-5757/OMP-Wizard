#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

int main() {
	//Initialize Stuff
	auto screen = ftxui::ScreenInteractive::TerminalOutput();

	//Component to render
	auto toRender = ftxui::Renderer([] {
		return ftxui::text("Hello World");
	});

	//Loop and render component on the screen
	screen.Loop(toRender);
}