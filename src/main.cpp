#include "advancedMode/advancedMode.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "shared/structs/Config.hpp"
#include "simpleMode/simpleMode.hpp"

using namespace std;
using namespace ftxui;

//@ Main entry point for OMP Wizard
//@ Initializes the TUI screens, manages mode switching, and runs the main interactive loop.
int main() {
    //* Initialize state and screen
    auto screen = ScreenInteractive::Fullscreen();
    ConfigState state;
    int tab_index = 0;

    //! --------------------Main Menu--------------------
    std::vector<std::string> mode_entries = { "Simple Mode", "Advanced Mode" };
    int mode_selected = 0;

    auto mode_radio = Radiobox(&mode_entries, &mode_selected);

    auto main_menu = Container::Vertical({ mode_radio });

    //* Setup renderer
    auto toRender = Renderer(main_menu, [&] {
        return vbox({ text("Welcome to OMP Wizard") | hcenter, text(" "), mode_radio->Render() | hcenter, text(" "), text(" "),
                      vbox({ text("[q] = Quit the wizard without generating config file"), text("[n] = Confirm selections & go to the next screen") })
                        | hcenter })
             | center;
    });

    //* Catch keypresses
    auto component = CatchEvent(toRender, [&](Event event) {
        // Quit on 'q'
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        // Advance to next screen or select mode on 'n'
        if (event == Event::Character('n')) {
            if (mode_selected == 0) {
                tab_index = 1;   // Simple Mode tab index
            } else {
                screen.ExitLoopClosure()();   // Exit screen loop to launch Advanced Mode CLI
                tab_index = 2;
                return true;
            }
        }
        return false;
    });

    //* Setup Simple Mode and navigation container
    auto simple_mode = MakeSimpleMode(state, screen.ExitLoopClosure());
    auto selector = Container::Tab({ component, simple_mode }, &tab_index);

    //* Run the screen loop
    screen.Loop(selector);

    //* Launch advanced CLI mode if selected
    if (tab_index == 2) {
        advancedMode();
    }
    return 0;
}
