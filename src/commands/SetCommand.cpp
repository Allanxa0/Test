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
#include "ll/api/coro/CoroTask.h"
#include "ll/api/thread/ServerThreadExecutor.h"
#include <algorithm>
#include <chrono>

using namespace ll::chrono_literals;

namespace my_mod {

struct SetParams {
    std::string pattern;
};

ll::coro::CoroTask<void> executeSetTask(Player* player, BlockPos p1, BlockPos p2, const Block* blockToSet, DimensionType dim) {
    int minX = std::min(p1.x, p2.x);
    int minY = std::min(p1.y, p2.y);
    int minZ = std::min(p1.z, p2.z);
    int maxX = std::max(p1.x, p2.x);
    int maxY = std::max(p1.y, p2.y);
    int maxZ = std::max(p1.z, p2.z);

    auto& region = player->getDimension().getBlockSourceFromMainChunkSource();
    BlockChangeContext context(true); 
    
    std::vector<BlockEdit> undoHistory;
    int estimatedCount = (maxX - minX + 1) * (maxY - minY + 1) * (maxZ - minZ + 1);
    undoHistory.reserve(estimatedCount);

    int count = 0;
    auto startTime = std::chrono::steady_clock::now();
    const auto timeBudget = std::chrono::milliseconds(30);

    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            for (int z = minZ; z <= maxZ; ++z) {
                BlockPos targetPos(x, y, z);
                const Block& oldBlock = region.getBlock(targetPos);
                
                std::unique_ptr<CompoundTag> oldNbt = nullptr;
                if (auto* actor = region.getBlockEntity(targetPos)) {
                    oldNbt = std::make_unique<CompoundTag>();
                    actor->save(*oldNbt);
                }

                undoHistory.push_back({targetPos, &oldBlock, std::move(oldNbt), dim});
                
                region.setBlock(targetPos, *blockToSet, 3, nullptr, context);
                
                count++;
                
                if (std::chrono::steady_clock::now() - startTime >= timeBudget) {
                    co_await 1_tick;
                    startTime = std::chrono::steady_clock::now();
                }
            }
        }
    }

    WorldEditMod::getInstance().getSessionManager().pushHistory(*player, {std::move(undoHistory)});
    player->sendMessage("§aOperation completed. " + std::to_string(count) + " blocks affected.");
    co_return;
}

void registerSetCommand() {
    auto& registrar = ll::command::CommandRegistrar::getInstance(false);

    auto& setCmd = registrar.getOrCreateCommand("set", "Set blocks in selection");
    setCmd.overload<SetParams>()
        .required("pattern")
        .execute([](CommandOrigin const& origin, CommandOutput& output, SetParams const& params) {
            auto* entity = origin.getEntity();
            if (!entity || !entity->isType(ActorType::Player)) {
                output.error("Only players can use this command");
                return;
            }

            auto* player = static_cast<Player*>(entity);
            if (!player->isOperator()) {
                output.error("You do not have permission to use this command.");
                return;
            }

            auto& session = WorldEditMod::getInstance().getSessionManager().getSelection(*player);

            if (!session.isComplete()) {
                output.error("Make a selection first.");
                return;
            }

            if (session.dimId.value() != player->getDimensionId()) {
                output.error("Selection is in a different dimension.");
                return;
            }

            std::string blockName = params.pattern;
            if (blockName.find(':') == std::string::npos) {
                blockName = "minecraft:" + blockName;
            }

            auto blockOpt = Block::tryGetFromRegistry(blockName);
            if (!blockOpt.has_value()) {
                output.error("Invalid block pattern: " + blockName);
                return;
            }
            
            const Block* blockToSet = &(*blockOpt);
            BlockPos p1 = session.pos1.value();
            BlockPos p2 = session.pos2.value();

            executeSetTask(player, p1, p2, blockToSet, session.dimId.value()).launch(ll::thread::ServerThreadExecutor::getDefault());
            
            output.success("§aSet operation started in the background...");
        });
}

}

