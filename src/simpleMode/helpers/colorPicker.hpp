#include "../helpers/rgbToHex.hpp"
#include "../structs/Config.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace std;
using namespace ftxui;

//@ To create tab components for color selector tabs
Component colorPicker(rgb* rgbValues, string& hex) {
    auto slider_r = Slider("Red   :", &rgbValues->red, 0, 255, 1);
    auto slider_g = Slider("Green :", &rgbValues->green, 0, 255, 1);
    auto slider_b = Slider("Blue  :", &rgbValues->blue, 0, 255, 1);

    auto layout = Container::Vertical({ slider_r, slider_g, slider_b });

    return Renderer(layout, [=, &hex] {
        auto preview = text("   COLOR   ") | bgcolor(Color::RGB(rgbValues->red, rgbValues->green, rgbValues->blue))
                     | color(Color::Black);
        hex = RGBtoHex(rgbValues->red, rgbValues->green, rgbValues->blue);

        // the size() options are necessary to make it wide enough.
        // or else it shrinks it too much (possibly cuz of hcenter in the main renderer)
        return hbox({ preview, separator(),
                      vbox({ slider_r->Render(), slider_g->Render(), slider_b->Render(), separator(),
                             text("Hex: " + hex) })
                        | size(WIDTH, GREATER_THAN, 30) })
             | border | size(WIDTH, GREATER_THAN, 30);
    });
}
