#pragma once

namespace my_mod {

struct Config {
    int wandCooldownMs = 400;
    int maxHistorySize = 10;
    int maxBlocksPerOperation = 500000;
    
    int version = 1;
};

}

