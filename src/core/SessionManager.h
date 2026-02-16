#pragma once
#include "Selection.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/block/Block.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <stack>

namespace my_mod {

struct BlockEdit {
    BlockPos pos;
    const Block* oldBlock;
    DimensionType dim;
};

struct EditAction {
    std::vector<BlockEdit> blocks;
};

class SessionManager {
public:
    Selection& getSelection(Player& player);
    
    void setPos1(Player& player, const BlockPos& pos);
    void setPos2(Player& player, const BlockPos& pos);
    void removeSelection(Player& player);

    void pushHistory(Player& player, std::vector<BlockEdit>&& edits);
    bool undo(Player& player);

private:
    std::unordered_map<std::string, Selection> mSessions;
    std::unordered_map<std::string, std::stack<EditAction>> mHistory;
};

}

