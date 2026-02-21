#include "WECommands.h"

namespace my_mod {

void registerPosCommands();
void registerWandCommand();
void registerSetCommand();
void registerUndoCommand();
void registerRedoCommand();
void registerCopyCommand();
void registerPasteCommand();

void WECommands::registerCommands() {
    registerPosCommands();
    registerWandCommand();
    registerSetCommand();
    registerUndoCommand();
    registerRedoCommand();
    registerCopyCommand();
    registerPasteCommand();
}

}

