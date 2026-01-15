#pragma once
#include <string>

//@ Helper to convert rgba values to HEX codes
std::string RGBtoHex(int r, int g, int b);

//@ Helper to convert HEX codes rgba values
bool HexToRGB(std::string hex, int& r, int& g, int& b);
