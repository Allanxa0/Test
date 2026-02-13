
#include "SessionManager.h"

namespace my_mod {

Selection& SessionManager::getSelection(Player& player) {
    return mSessions[player.getXuid()];
}

void SessionManager::setPos1(Player& player, const BlockPos& pos) {
    mSessions[player.getXuid()].pos1 = pos;
}

void SessionManager::setPos2(Player& player, const BlockPos& pos) {
    mSessions[player.getXuid()].pos2 = pos;
}

}


