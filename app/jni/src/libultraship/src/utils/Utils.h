#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace LUS {

namespace Math {
float clamp(float d, float min, float max);
template <typename Numeric> bool IsNumber(const std::string& s) {
    Numeric n;
    return ((std::istringstream(s) >> n >> std::ws).eof());
}
} // namespace Math

std::vector<std::string> splitText(const std::string& text, char separator, bool keepQuotes);
std::string toLowerCase(std::string in);
} // namespace LUS
