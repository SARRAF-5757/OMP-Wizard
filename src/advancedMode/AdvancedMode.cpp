#include "AdvancedMode.hpp"

#include <ftxui/dom/elements.hpp>

#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"

using namespace ftxui;

Component MakeAdvancedMode(ConfigState& config, function<void()> on_quit) {
    auto toRender = Renderer([] {
        return vbox({ text("Advanced Mode Coming Soon"), text(""),
                      text("[q] = Quit the wizard without generating config file") })
             | hcenter | vcenter;
    });

    auto component = CatchEvent(toRender, [=, &config](Event event) {
        if (event == Event::Character('q')) {
            on_quit();
            return true;
        }
        return false;
    });

    return component;
}
