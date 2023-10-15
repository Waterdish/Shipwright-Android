#pragma once

#include <string>
#include <vector>
#include <memory>

namespace LUS {
class Archive;

struct File {
    std::shared_ptr<Archive> Parent;
    std::string Path;
    std::vector<char> Buffer;
    bool IsLoaded = false;
};
} // namespace LUS
