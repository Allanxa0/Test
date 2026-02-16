#include "SessionManager.h"
#include "mc/world/level/dimension/Dimension.h"
#include "mc/world/level/BlockSource.h"

namespace my_mod {

SessionManager& SessionManager::getInstance() {
    static SessionManager instance;
    return instance;
}

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
    mUndoHistory.erase(player.getXuid());
    mRedoHistory.erase(player.getXuid());
}

void SessionManager::pushHistory(Player& player, EditAction&& action) {
    mUndoHistory[player.getXuid()].push(std::move(action));
    while (!mRedoHistory[player.getXuid()].empty()) {
        mRedoHistory[player.getXuid()].pop();
    }
}

std::optional<EditAction> SessionManager::popUndo(Player& player) {
    auto& history = mUndoHistory[player.getXuid()];
    if (history.empty()) return std::nullopt;

    auto action = std::move(history.top());
    history.pop();
    return action;
}

void SessionManager::pushRedo(Player& player, EditAction&& action) {
    mRedoHistory[player.getXuid()].push(std::move(action));
}

std::optional<EditAction> SessionManager::popRedo(Player& player) {
    auto& history = mRedoHistory[player.getXuid()];
    if (history.empty()) return std::nullopt;

    auto action = std::move(history.top());
    history.pop();
    return action;
}

}

