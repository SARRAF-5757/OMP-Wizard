#include "advancedMode/advancedMode.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "simpleMode/simpleMode.hpp"
#include "shared/structs/Config.hpp"

using namespace std;
using namespace ftxui;

int main() {
    auto screen = ScreenInteractive::Fullscreen();
    ConfigState state;
    int tab_index = 0;

    //! --------------------Main Menu--------------------
    std::vector<std::string> mode_entries = { "Simple Mode", "Advanced Mode" };
    int mode_selected = 0;

    auto mode_radio = Radiobox(&mode_entries, &mode_selected);

    auto main_menu = Container::Vertical({ mode_radio });

    auto toRender = Renderer(main_menu, [&] {
        return vbox({ text("Welcome to OMP Wizard") | hcenter, text(" "), mode_radio->Render() | hcenter, text(" "), text(" "),
                      vbox({ text("[q] = Quit the wizard without generating config file"), text("[n] = Confirm selections & go to the next screen") })
                        | hcenter })
             | center;
    });

    auto component = CatchEvent(toRender, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Character('n')) {
            if (mode_selected == 0) {
                tab_index = 1;
            } else {
                screen.ExitLoopClosure()();
                tab_index = 2;
                return true;
            }
        }
        return false;
    });
    auto simple_mode = MakeSimpleMode(state, screen.ExitLoopClosure());
    auto selector = Container::Tab({ component, simple_mode }, &tab_index);

    screen.Loop(selector);
    if (tab_index == 2) {
        advancedMode();
    }
    return 0;
}
