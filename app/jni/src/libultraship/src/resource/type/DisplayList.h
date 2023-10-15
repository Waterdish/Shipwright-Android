#pragma once

#include <vector>
#include "resource/Resource.h"
#include "libultraship/libultra/gbi.h"

namespace LUS {
class DisplayList : public Resource<Gfx> {
  public:
    using Resource::Resource;

    DisplayList();

    Gfx* GetPointer() override;
    size_t GetPointerSize() override;

    std::vector<Gfx> Instructions;
};
} // namespace LUS
