# OMP Wizard

This is a visual wizard to help users set up and customize their [Oh-My-Posh](https://github.com/JanDeDobbeleer/oh-my-posh) prompts.

# Getting Started

1. Download the binary from [Releases](https://github.com/SARRAF-5757/OMP-Wizard/releases) page
2. Navigate to the directory (ex. Downloads) the binary's in
3. Give executable permissions by `chmod +x .omp-wizaard`
4. Run the wizard with `omp-wizaard`

# Feature Checklist

- [x] Left prompt customization
  - [x] User
  - [x] Directory Path
  - [x] Git status
  - [ ] More blocks coming!
- [ ] Right prompt customization
- [x] Three different color modes (full/text/monochrome)
- [x] Color Picker
- [x] Transient Prompt support
- [x] Transient Prompt customization
- [ ] Tab title customization
- [ ] Live render of prompt's current customization progress
- [ ] Agnoster support for path block
- [ ] Icon-less mode
- [ ] Advanced Mode
- [ ] More customization!

# Build your own binary

1. Clone repository
2. `mkdir build`
3. `cd build`
4. `cmake ..`
5. `make`
6. `./omp-wizaard`

## References

- TUI library used - [FTXUI](https://github.com/ArthurSonzogni/FTXUI) | [Docs](https://arthursonzogni.github.io/FTXUI/index.html)
- Library used to generate JSON - [nlohmann/json](https://github.com/nlohmann/json) | [Docs](https://json.nlohmann.me/api/basic_json/)
- Find [Diamonds](https://www.nerdfonts.com/cheat-sheet)
  - [Powerline symbols](https://github.com/ryanoasis/powerline-extra-symbols/raw/master/img/fontforge.png)
  - To check what a diamond looks like, run `print '\u[hex code for icon]'` in the terminal (`\uef4d` for example)

