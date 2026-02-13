#include "WECommands.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/command/CommandHandle.h"

namespace my_mod {

void registerPosCommands();
void registerWandCommand();
void registerSetCommand();

void WECommands::registerCommands() {
    registerPosCommands();
    registerWandCommand();
    registerSetCommand();
}

}

