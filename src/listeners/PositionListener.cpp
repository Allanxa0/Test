#include "PositionListener.h"
#include "WorldEditMod.h"
#include "ll/api/event/EventBus.h"
#include "BlockBreakEvent.h"
#include "BlockPlayerInteractEvent.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/item/Item.h"
#include "mc/world/actor/player/Player.h"

namespace my_mod {

void PositionListener::registerListeners() {
    auto& bus = ll::event::EventBus::getInstance();

    bus.addListener<BlockEvents::BlockBreakEvent>(
        [](BlockEvents::BlockBreakEvent& ev) {
            
        }
    );

    bus.addListener<BlockEvents::BlockPlayerInteractEvent>(
        [](BlockEvents::BlockPlayerInteractEvent& ev) {
            auto& player = ev.mPlayer;
            auto& item = player.getSelectedItem();
            
            if (!item.isNull() && item.getItem()->getRawNameId() == "minecraft:wooden_axe") {
                WorldEditMod::getInstance().getSessionManager().setPos2(player, ev.mHit->mBlockPos);
                player.sendMessage("Second position set to " + ev.mHit->mBlockPos.toString());
                ev.mSuccessful = true;
            }
        }
    );
}

}

