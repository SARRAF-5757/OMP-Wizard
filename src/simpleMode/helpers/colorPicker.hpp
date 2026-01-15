#include "../helpers/convertColors.hpp"
#include "../../shared/structs/Config.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace std;
using namespace ftxui;

//@ To create tab components for color selector tabs
Component colorPicker(rgb* rgbValues, string& hex) {
    //* Color Sliders
    auto slider_r = Slider("Red   :", &rgbValues->red, 0, 255, 1);
    auto slider_g = Slider("Green :", &rgbValues->green, 0, 255, 1);
    auto slider_b = Slider("Blue  :", &rgbValues->blue, 0, 255, 1);

    //* Text area (Input)
    InputOption hexOption;
    hexOption.on_change = [=, &hex] {   // when text box changes, update rgbValues
        int r, g, b;

        if (HexToRGB(hex, r, g, b)) {
            rgbValues->red = r;
            rgbValues->green = g;
            rgbValues->blue = b;
        }
    };

    //* Setup UI
    auto hexInput = Input(&hex, "FFFFFF", hexOption);
    auto layout = Container::Vertical({ slider_r, slider_g, slider_b, hexInput });

    return Renderer(layout, [=, &hex] {
        auto preview = text("   COLOR   ") | bgcolor(Color::RGB(rgbValues->red, rgbValues->green, rgbValues->blue)) | color(Color::Black);

        string calculatedHex = RGBtoHex(rgbValues->red, rgbValues->green, rgbValues->blue);

        if (!hexInput->Focused()) {   // Only overwrite the input box when not actively typing
            hex = calculatedHex;
        }

        // the size() options are necessary to make it wide enough.
        // or else it shrinks it too much (possibly cuz of hcenter in the main renderer)
        return hbox({ preview, separator(),
                      vbox({ slider_r->Render(), slider_g->Render(), slider_b->Render(), separator(),
                             hbox({ text("Hex: "), hexInput->Render() | size(WIDTH, GREATER_THAN, 10) }) })
                        | size(WIDTH, GREATER_THAN, 30) })
             | border | size(WIDTH, GREATER_THAN, 30);
    });
}
