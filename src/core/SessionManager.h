
#pragma once
#include "Selection.h"
#include "mc/world/actor/player/Player.h"
#include <unordered_map>
#include <string>

namespace my_mod {

class SessionManager {
public:
    Selection& getSelection(Player& player);
    
    void setPos1(Player& player, const BlockPos& pos);
    void setPos2(Player& player, const BlockPos& pos);

private:
    std::unordered_map<std::string, Selection> mSessions;
};

}

