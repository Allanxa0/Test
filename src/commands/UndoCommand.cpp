#include "WorldEditMod.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/Command.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/world/actor/player/Player.h"

namespace my_mod {

void registerUndoCommand() {
    auto& registrar = ll::command::CommandRegistrar::getInstance(false);
    auto& undoCmd = registrar.getOrCreateCommand("undo", "Undo last operation");

    undoCmd.overload().execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* entity = origin.getEntity();
        if (!entity || !entity->isType(ActorType::Player)) {
            output.error("Only players can use this command");
            return;
        }

        auto* player = static_cast<Player*>(entity);
        bool success = WorldEditMod::getInstance().getSessionManager().undo(*player);

        if (success) {
            output.success("Undid last operation.");
        } else {
            output.error("Nothing to undo.");
        }
    });
}

}

