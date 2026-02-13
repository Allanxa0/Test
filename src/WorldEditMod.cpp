
#include "WorldEditMod.h"
#include "commands/WECommands.h"
#include "ll/api/mod/RegisterHelper.h"

namespace my_mod {

WorldEditMod& WorldEditMod::getInstance() {
    static WorldEditMod instance;
    return instance;
}

bool WorldEditMod::load() {
    return true;
}

bool WorldEditMod::enable() {
    WECommands::registerCommands();
    return true;
}

bool WorldEditMod::disable() {
    return true;
}

}

LL_REGISTER_MOD(my_mod::WorldEditMod, my_mod::WorldEditMod::getInstance());

