#pragma once
#include "Selection.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/block/Block.h"
#include "mc/nbt/CompoundTag.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <optional>

namespace my_mod {

struct BlockEdit {
    BlockPos pos;
    const Block* oldBlock;
    std::unique_ptr<CompoundTag> oldNbt;
    DimensionType dim;
};

struct EditAction {
    std::vector<BlockEdit> blocks;
};

class SessionManager {
public:
    static SessionManager& getInstance(); 

    Selection& getSelection(Player& player);
    
    void setPos1(Player& player, const BlockPos& pos);
    void setPos2(Player& player, const BlockPos& pos);
    void removeSelection(Player& player);

    void pushHistory(Player& player, EditAction&& action);
    std::optional<EditAction> popUndo(Player& player);
    
    void pushRedo(Player& player, EditAction&& action);
    std::optional<EditAction> popRedo(Player& player);

private:
    std::unordered_map<std::string, Selection> mSessions;
    std::unordered_map<std::string, std::stack<EditAction>> mUndoHistory;
    std::unordered_map<std::string, std::stack<EditAction>> mRedoHistory;
};

}

