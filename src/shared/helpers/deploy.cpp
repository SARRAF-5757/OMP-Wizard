#include "deploy.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

// Platform-specific headers for process handling
#ifdef _WIN32
#include <windows.h>
#define POPEN _popen
#define PCLOSE _pclose
#else
#include <unistd.h>
#define POPEN popen
#define PCLOSE pclose
#endif


//@ Returns the platform-appropriate path for the generated OMP configuration.
//@ Windows: %LOCALAPPDATA%\oh-my-posh\omp-wizard.json
//@ Unix: ~/.config/oh-my-posh/omp-wizard.json
std::filesystem::path getConfigPath() {
#ifdef _WIN32
    const char* localAppData = std::getenv("LOCALAPPDATA");
    if (localAppData) {
        return std::filesystem::path(localAppData) / "oh-my-posh" / "omp-wizard.json";
    }
    return std::filesystem::path("C:/") / "omp-wizard.json";
#else
    const char* home = std::getenv("HOME");
    if (home) {
        return std::filesystem::path(home) / ".config" / "oh-my-posh" / "omp-wizard.json";
    }
    return std::filesystem::path("/tmp") / "omp-wizard.json";
#endif
}

//@ Determine the active shell name
std::string getActiveShell() {
    std::array<char, 128> buffer;
    std::string result;
    FILE* pipe = POPEN("oh-my-posh get shell", "r");

    // read shell name into result through the pipe
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    PCLOSE(pipe);

    // Remove whitespaces
    result.erase(std::remove_if(result.begin(), result.end(), [](unsigned char x) { return std::isspace(x); }), result.end());

    // If command fails try the environment variable fallbacks
    if (result.empty() || result.find("notfound") != std::string::npos || result.find("commandnotfound") != std::string::npos) {
        const char* envShell = std::getenv("SHELL");
        if (envShell) {
            std::string s(envShell);
            if (s.find("zsh") != std::string::npos) return "zsh";
            if (s.find("bash") != std::string::npos) return "bash";
            if (s.find("fish") != std::string::npos) return "fish";
        }
        return "";
    }

    return result;
}

//@ Map shell name to configuration path
std::filesystem::path getProfilePath(const std::string& shell) {
    const char* home = std::getenv("HOME");
#ifdef _WIN32
    if (shell == "pwsh" || shell == "powershell") {
        const char* userProfile = std::getenv("USERPROFILE");
        if (userProfile) {
            // PowerShell profiles are usually in Documents\PowerShell
            std::filesystem::path docPath = std::filesystem::path(userProfile) / "Documents";
            std::filesystem::path profile = (shell == "pwsh") ? docPath / "PowerShell" / "Microsoft.PowerShell_profile.ps1"
                                                              : docPath / "WindowsPowerShell" / "Microsoft.PowerShell_profile.ps1";
            return profile;
        }
    }
#else
    if (!home) return "";
    if (shell == "bash") return std::filesystem::path(home) / ".bashrc";
    if (shell == "zsh") return std::filesystem::path(home) / ".zshrc";
    if (shell == "fish") return std::filesystem::path(home) / ".config" / "fish" / "config.fish";
#endif
    return "";
}

//@ Append Oh-My-Posh setup line to shell profile file
bool updateProfile(const std::filesystem::path& profilePath, const std::filesystem::path& configPath, const std::string& shell) {
    if (profilePath.empty()) {
        return false;
    }

    //* Construct the initialization string
    std::string initString;
    if (shell == "bash" || shell == "zsh") {
        initString = "eval \"$(oh-my-posh init " + shell + " --config '" + configPath.string() + "')\"";
    } else if (shell == "fish") {
        initString = "oh-my-posh init fish --config '" + configPath.string() + "' | source";
    } else if (shell == "pwsh" || shell == "powershell") {
        initString = "oh-my-posh init " + shell + " --config '" + configPath.string() + "' | Invoke-Expression";
    } else {
        std::cerr << "Error: Unsupported shell: '" << shell << std::endl;
        return false;
    }

    //* Duplicate line check in shell file
    if (std::filesystem::exists(profilePath)) {
        std::ifstream profileRead(profilePath);
        if (profileRead.is_open()) {
            std::string line;
            while (std::getline(profileRead, line)) {
                if (line.find(configPath.string()) != std::string::npos) {
                    return true;
                }
            }
            profileRead.close();
        }
    }

    //* Create necessary parent directories
    std::error_code ec;
    std::filesystem::create_directories(profilePath.parent_path(), ec);

    //* Add the config line to the end of the file
    std::ofstream profileWrite(profilePath, std::ios_base::app);
    if (!profileWrite.is_open()) {
        return false;
    }

    profileWrite << "\n# Oh My Posh configuration wizard generated entry\n";
    profileWrite << initString << "\n";
    profileWrite.close();

    return true;
}

//! pseudo int main; processes the json response and writes it into a file in the right place based on the OS
bool deployConfig(const std::string& configJson) {
    //* Save the (json) configuration file
    std::filesystem::path configPath = getConfigPath();
    std::filesystem::create_directories(configPath.parent_path());

    std::ofstream o(configPath);
    if (!o.is_open()) return false;
    o << configJson;
    o.close();

    //* Identify and update the shell profile
    std::string shell = getActiveShell();

    std::filesystem::path profilePath = getProfilePath(shell);
    if (profilePath.empty()) {
        return false;
    }

    if (updateProfile(profilePath, configPath, shell)) {
        return true;
    }

    return false;
}
