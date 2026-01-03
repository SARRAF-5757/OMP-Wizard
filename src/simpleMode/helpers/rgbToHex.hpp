#pragma once
#include <iomanip>
#include <sstream>
#include <string>


using namespace std;

//@ Helper to convert rgba values to HEX codes
string RGBtoHex(int r, int g, int b) {
    stringstream ss;
    ss << "#" << hex << setfill('0') << setw(2) << r << setw(2) << g << setw(2) << b;
    return ss.str();
}
