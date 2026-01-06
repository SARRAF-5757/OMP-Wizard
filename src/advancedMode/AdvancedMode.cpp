#include "AdvancedMode.hpp"

#include <ftxui/dom/elements.hpp>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"

using namespace ftxui;

Component MakeAdvancedMode(ConfigState& config, std::function<void()> on_quit) {
    // Add a focusable child so the component tree can receive keyboard events
    auto quit_button = Button("Quit", [=] { on_quit(); });
    auto container = Container::Vertical({ quit_button });

    auto toRender = Renderer(container, [container] {
        return vbox({ container->Render() | hcenter, text("Advanced Mode Coming Soon"), text(""),
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
