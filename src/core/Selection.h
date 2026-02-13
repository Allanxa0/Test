
#pragma once
#include "mc/world/level/BlockPos.h"
#include <optional>

namespace my_mod {

struct Selection {
    std::optional<BlockPos> pos1;
    std::optional<BlockPos> pos2;

    bool isComplete() const {
        return pos1.has_value() && pos2.has_value();
    }
};

}

