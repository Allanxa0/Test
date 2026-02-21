#include "WorldEditMod.h"
#include "commands/WECommands.h"
#include "listeners/PositionListener.h"
#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/Listener.h"
#include "ll/api/event/player/PlayerDisconnectEvent.h"
#include "ll/api/Config.h"

namespace my_mod {

WorldEditMod& WorldEditMod::getInstance() {
    static WorldEditMod instance;
    return instance;
}

bool WorldEditMod::load() {
    ll::config::loadConfig(mConfig, getSelf().getConfigDir() / "config.json");
    ll::config::saveConfig(mConfig, getSelf().getConfigDir() / "config.json");
    return true;
}

bool WorldEditMod::enable() {
    WECommands::registerCommands();
    PositionListener::registerListeners();

    auto& bus = ll::event::EventBus::getInstance();
    auto disconnectListener = ll::event::Listener<ll::event::player::PlayerDisconnectEvent>::create([](ll::event::player::PlayerDisconnectEvent& ev) {
        WorldEditMod::getInstance().getSessionManager().onPlayerLeft(ev.self());
    });
    bus.addListener<ll::event::player::PlayerDisconnectEvent>(disconnectListener);

    return true;
}

bool WorldEditMod::disable() {
    return true;
}

}

LL_REGISTER_MOD(my_mod::WorldEditMod, my_mod::WorldEditMod::getInstance());

