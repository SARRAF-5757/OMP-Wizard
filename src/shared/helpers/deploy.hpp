#ifndef DEPLOY_HPP
#define DEPLOY_HPP

#include <filesystem>
#include <string>

//@ Detects the OS and returns the default path for the OMP wizard config
std::filesystem::path getConfigPath();

//@ Executes 'oh-my-posh get shell' and returns the active shell name
std::string getActiveShell();

//@ Maps the detected shell to the correct user profile file path
std::filesystem::path getProfilePath(const std::string& shell);

//@ Safely appends the OMP initialization string to the profile file
bool updateProfile(const std::filesystem::path& profilePath, const std::filesystem::path& configPath, const std::string& shell);

//@ Main deployment function. Saves the JSON and updates the shell profile
bool deployConfig(const std::string& configJson);

#endif
