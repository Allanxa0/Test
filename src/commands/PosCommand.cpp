
#include "WorldEditMod.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/command/Command.h"
#include "mc/world/actor/player/Player.h"

namespace my_mod {

void registerPosCommands() {
    auto& registrar = ll::command::CommandRegistrar::getInstance();

    auto pos1Cmd = registrar.getOrCreateCommand("pos1", "Set position 1");
    pos1Cmd.overload().execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* entity = origin.getEntity();
        if (!entity || !entity->isType(ActorType::Player)) {
            output.error("Only players can use this command");
            return;
        }

        auto* player = static_cast<Player*>(entity);
        BlockPos pos = player->getBlockPos();
        
        WorldEditMod::getInstance().getSessionManager().setPos1(*player, pos);
        
        output.success("First position set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
    });

    auto pos2Cmd = registrar.getOrCreateCommand("pos2", "Set position 2");
    pos2Cmd.overload().execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* entity = origin.getEntity();
        if (!entity || !entity->isType(ActorType::Player)) {
            output.error("Only players can use this command");
            return;
        }

        auto* player = static_cast<Player*>(entity);
        BlockPos pos = player->getBlockPos();

        WorldEditMod::getInstance().getSessionManager().setPos2(*player, pos);

        output.success("Second position set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
    });
}

}

