#include <iomanip>
#include <sstream>

//@ Helper to convert rgba values to HEX codes
std::string RGBtoHex(int r, int g, int b) {
    std::stringstream ss;
    ss << "#" << std::hex << std::setfill('0') << std::setw(2) << r << std::setw(2) << g << std::setw(2) << b;
    return ss.str();
}

//@ Helper to convert HEX codes rgba values
bool HexToRGB(std::string hex, int& r, int& g, int& b) {
    if (hex.empty()) return false;
    if (hex[0] == '#') hex.erase(0, 1);
    if (hex.length() != 6) return false;

    int r_val = stoi(hex.substr(0, 2), nullptr, 16);
    int g_val = stoi(hex.substr(2, 2), nullptr, 16);
    int b_val = stoi(hex.substr(4, 2), nullptr, 16);
    r = r_val;
    g = g_val;
    b = b_val;
    return true;
}
