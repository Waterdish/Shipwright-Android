#include "Utils.h"
#include <cstring>
#include <algorithm>

#ifdef _MSC_VER
#define strdup _strdup
#endif

namespace LUS {
namespace Math {
float clamp(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

} // namespace Math

std::vector<std::string> splitText(const std::string text, char separator = ' ', bool keepQuotes = false) {
    std::vector<std::string> args;
    char* input = strdup(text.c_str());
    const size_t length = strlen(input);

    bool inQuotes = false;
    size_t count = 0, from = 0;

    for (size_t i = 0; i < length; i++) {
        if (input[i] == '"') {
            inQuotes = !inQuotes;
        } else if (input[i] == separator && !inQuotes) {
            size_t strlen = i - from;

            if (strlen > 0) {
                if (!keepQuotes && input[from] == '"' && input[i - 1] == '"') {
                    from++;
                    strlen -= 2;
                }

                char* tmp = new char[strlen + 1]();
                strncpy(tmp, &input[from], strlen);
                count++;
                args.emplace_back(tmp);
            }

            from = i + 1;
        }
    }

    if (from < length) {
        size_t strlen = length - from;

        if (!keepQuotes && input[from] == L'"' && input[length - 1] == L'"') {
            from++;
            strlen -= 2;
        }

        char* tmp = new char[strlen + 1]();
        strncpy(tmp, &input[from], strlen);
        count++;
        args.emplace_back(tmp);
    }

    return args;
}

std::string toLowerCase(std::string in) {
    std::string cpy(in);
    std::transform(cpy.begin(), cpy.end(), cpy.begin(), ::tolower);
    return cpy;
}
} // namespace LUS
