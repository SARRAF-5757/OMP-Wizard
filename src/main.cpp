#include "advancedMode/advancedMode.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "shared/structs/Config.hpp"
#include "simpleMode/simpleMode.hpp"

using namespace std;
using namespace ftxui;

//@ Initializes the TUI screens, manages mode switching, and runs the main interactive loop.
int main() {
    //* Initialize state and screen
    auto screen = ScreenInteractive::Fullscreen();
    ConfigState state;
    int tab_index = 0;

    //! --------------------Main Menu--------------------
    std::vector<std::string> mode_entries = { "Simple Mode", "Advanced Mode" };
    int mode_selected = 0;

    //* Define radiobox customizations (options)
    RadioboxOption mode_radio_option = RadioboxOption::Simple();
    auto default_mode_radio_transform = mode_radio_option.transform;
    mode_radio_option.transform = [default_mode_radio_transform](const EntryState& s) {
        auto element = default_mode_radio_transform(s);
        return vbox({ text(""), element, text("") });
    };

    auto mode_radio = Radiobox(RadioboxOption { mode_entries, &mode_selected, mode_radio_option.transform });

    //* Define button customizations (options)
    ButtonOption quit_option;
    quit_option.transform = [](const EntryState& s) {
        auto element = text(s.label) | border;
        if (s.focused) {
            return element | bgcolor(Color::Red) | color(Color::Black) | bold;
        } else {
            return element | color(Color::MediumPurple);
        }
    };

    ButtonOption confirm_option;
    confirm_option.transform = [](const EntryState& s) {
        auto element = text(s.label) | border;
        if (s.focused) {
            return element | bgcolor(Color::Green) | color(Color::Black) | bold;
        } else {
            return element | color(Color::MediumPurple);
        }
    };

    //* Define navigation buttons
    auto btn_quit = Button("[q]", [&] { screen.ExitLoopClosure()(); }, quit_option);
    auto btn_confirm = Button(
      "[n]",
      [&] {
          if (mode_selected == 0) {
              tab_index = 1;   // Simple Mode tab index
          } else {
              screen.ExitLoopClosure()();   // Exit screen loop to launch Advanced Mode CLI
              tab_index = 2;
          }
      },
      confirm_option);

    auto buttons_vbox = Container::Vertical({
      btn_quit,
      btn_confirm,
    });

    auto main_menu = Container::Vertical({
      mode_radio,
      buttons_vbox,
    });

    //* Helper to vertically center button explanation text to align with 3-line buttons
    auto explanation_line = [](std::string text_str) { return vbox({ text(""), text(text_str), text("") }); };

    //* Setup renderer
    auto toRender = Renderer(main_menu, [&] {
        return vbox({ text("Welcome to OMP Wizard") | bold | hcenter, text(" "), mode_radio->Render() | hcenter, text(" "), text(" "),
                      hbox({ buttons_vbox->Render(), vbox({ explanation_line(" = Quit the wizard without generating config file"),
                                                            explanation_line(" = Confirm selections & go to the next screen") }) })
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

    //* Wrap selector in a root renderer to put a persistent wireframe around the TUI
    auto root_renderer = Renderer(selector, [&] { return selector->Render() | border; });

    //* Run the screen loop
    screen.Loop(root_renderer);

    //* Launch advanced CLI mode if selected
    if (tab_index == 2) {
        advancedMode();
    }
    return 0;
}
