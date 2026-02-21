#include "WorldEditMod.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/Command.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/dimension/Dimension.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/block/BlockChangeContext.h"
#include "mc/world/level/block/actor/BlockActor.h"
#include "mc/world/item/SaveContext.h"
#include "ll/api/coro/CoroTask.h"
#include "ll/api/thread/ServerThreadExecutor.h"
#include "ll/api/chrono/GameChrono.h"
#include "mc/dataloadhelper/NewUniqueIdsDataLoadHelper.h"
#include "mc/world/level/Level.h"

namespace my_mod {

ll::coro::CoroTask<void> executePasteTask(Player* player) {
    auto& clipboard = WorldEditMod::getInstance().getSessionManager().getClipboard(*player);
    if (clipboard.empty()) {
        player->sendMessage("§cClipboard is empty.");
        co_return;
    }

    if (clipboard.size() > WorldEditMod::getInstance().getConfig().maxBlocksPerOperation) {
        player->sendMessage("§Paste exceeds maximum block limit.");
        co_return;
    }

    auto& region = player->getDimension().getBlockSourceFromMainChunkSource();
    BlockChangeContext context(true); 
    BlockPos playerPos(player->getPosition());
    DimensionType dim = player->getDimensionId();

    std::vector<BlockEdit> undoHistory;
    undoHistory.reserve(clipboard.size());

    auto startTime = std::chrono::steady_clock::now();
    auto measureStart = std::chrono::high_resolution_clock::now();
    const auto timeBudget = std::chrono::milliseconds(25);

    for (const auto& item : clipboard) {
        BlockPos targetPos = playerPos + item.offset;
        
        if (!region.hasChunksAt(targetPos, 0, false)) continue;

        const Block& oldBlock = region.getBlock(targetPos);
        
        std::unique_ptr<CompoundTag> oldNbt = nullptr;
        if (auto* actor = region.getBlockEntity(targetPos)) {
            oldNbt = std::make_unique<CompoundTag>();
            actor->save(*oldNbt, SaveContext());
            region.removeBlockEntity(targetPos);
        }

        undoHistory.push_back({targetPos, &oldBlock, std::move(oldNbt), dim});
        
        region.setBlock(targetPos, *item.block, 3, nullptr, context);

        if (item.nbt) {
            if (auto* actor = region.getBlockEntity(targetPos)) {
                NewUniqueIdsDataLoadHelper helper;
                helper.mLevel = &player->getLevel();
                
                auto clonedNbt = std::make_unique<CompoundTag>(*item.nbt);
                (*clonedNbt)["x"] = targetPos.x;
                (*clonedNbt)["y"] = targetPos.y;
                (*clonedNbt)["z"] = targetPos.z;

                actor->load(player->getLevel(), *clonedNbt, helper);
                actor->onChanged(region);
                actor->refresh(region);
            }
        }

        if (std::chrono::steady_clock::now() - startTime >= timeBudget) {
            co_await ll::chrono::ticks(1);
            startTime = std::chrono::steady_clock::now();
        }
    }

    WorldEditMod::getInstance().getSessionManager().pushHistory(*player, {std::move(undoHistory)});
    
    auto measureEnd = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(measureEnd - measureStart).count();

    player->sendMessage("§aPasted " + std::to_string(clipboard.size()) + " blocks. Time taken: " + std::to_string(duration) + "ms");
    co_return;
}

void registerPasteCommand() {
    auto& registrar = ll::command::CommandRegistrar::getInstance(false);
    auto& pasteCmd = registrar.getOrCreateCommand("paste", "Paste clipboard");

    pasteCmd.overload().execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* entity = origin.getEntity();
        if (!entity || !entity->isType(ActorType::Player)) return;
        auto* player = static_cast<Player*>(entity);

        executePasteTask(player).launch(ll::thread::ServerThreadExecutor::getDefault());
        output.success("Paste operation started...");
    });
}

}

