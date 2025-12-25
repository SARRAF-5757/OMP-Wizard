<p align="center">
  <img src="https://github.com/SARRAF-5757/OMP-Wizard/blob/main/images/Logo.png?raw=true" alt="OMP-Wizard Logo" width="50%"/>
</p>

# OMP Wizard

A visual wizard to help users set up and customize their [Oh-My-Posh](https://github.com/JanDeDobbeleer/oh-my-posh) prompts.

<p align="center">
  <img src="https://github.com/SARRAF-5757/OMP-Wizard/blob/main/images/color-modes.png?raw=true" alt="color-modes" width="48%"/>
  <img src="https://github.com/SARRAF-5757/OMP-Wizard/blob/main/images/main-screen.png?raw=true" alt="main-screen" width="48%"/>
</p>
<p align="center">
  <img src="https://github.com/SARRAF-5757/OMP-Wizard/blob/main/images/choose-diamond.png?raw=true" alt="choose-diamond" width="48%"/>
  <img src="https://github.com/SARRAF-5757/OMP-Wizard/blob/main/images/color-picker.png?raw=true" alt="color-picker" width="48%"/>
</p>

# Getting Started

1. Download the binary for your OS (Mac/Windows/Linux) from the [Releases](https://github.com/SARRAF-5757/OMP-Wizard/releases) page
2. Navigate to the directory (ex. Downloads) the file is in
3. Run the wizard with-
   a. `./mac-omp-wizard` for Mac & Linux
   b. `.\win-omp-wizard` for Windows
4. Follow on-screen directions

# Feature Checklist

- [x] Left prompt customization
  - [x] User
  - [x] Directory Path
  - [x] Git Status
  - [ ] More blocks coming!
- [ ] Right prompt customization
  - [ ] Time
  - [ ] Shell
  - [ ] Dynamic Tooling blocks
  - [ ] More coming
- [x] Three color modes (full/text/monochrome)
- [x] Color Picker
  - [ ] Manual HEX entry
- [x] Transient Prompt support
  - [ ] Transient Prompt customization
- [x] Tab title customization
- [ ] Live render of prompt's current customization progress
- [ ] Agnoster support for path block
- [ ] Icon-less mode
- [ ] Advanced Mode
- [x] Windows & Linux support
- [ ] Unified and streamlined setup process for all platforms
- [ ] More customization!

## References

- TUI library used - [FTXUI](https://github.com/ArthurSonzogni/FTXUI) | [Docs](https://arthursonzogni.github.io/FTXUI/index.html)
- Library used to generate JSON - [nlohmann/json](https://github.com/nlohmann/json) | [Docs](https://json.nlohmann.me/api/basic_json/)
- Find [Diamonds](https://www.nerdfonts.com/cheat-sheet)
  - [Powerline symbols](https://github.com/ryanoasis/powerline-extra-symbols/raw/master/img/fontforge.png)
  - To check what a diamond looks like, run `print '\u[hex code for icon]'` in the terminal (`\uef4d` for example)
