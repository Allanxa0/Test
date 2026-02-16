#include "SessionManager.h"
#include "mc/world/level/dimension/Dimension.h"
#include "mc/world/level/BlockSource.h"

namespace my_mod {

Selection& SessionManager::getSelection(Player& player) {
    return mSessions[player.getXuid()];
}

void SessionManager::setPos1(Player& player, const BlockPos& pos) {
    auto& sel = mSessions[player.getXuid()];
    sel.pos1 = pos;
    sel.dimId = player.getDimensionId();
}

void SessionManager::setPos2(Player& player, const BlockPos& pos) {
    auto& sel = mSessions[player.getXuid()];
    sel.pos2 = pos;
    sel.dimId = player.getDimensionId();
}

void SessionManager::removeSelection(Player& player) {
    mSessions.erase(player.getXuid());
    mHistory.erase(player.getXuid());
}

void SessionManager::pushHistory(Player& player, std::vector<BlockEdit>&& edits) {
    mHistory[player.getXuid()].push({std::move(edits)});
}

bool SessionManager::undo(Player& player) {
    auto& history = mHistory[player.getXuid()];
    if (history.empty()) return false;

    auto action = std::move(history.top());
    history.pop();

    auto& region = player.getDimension().getBlockSourceFromMainChunkSource();
    BlockChangeContext context(true);

    for (const auto& edit : action.blocks) {
        if (edit.dim == player.getDimensionId()) {
            region.setBlock(edit.pos, *edit.oldBlock, 3, nullptr, context);
        }
    }
    return true;
}

}

