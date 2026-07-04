#include <iostream>
#include "gamestate/GameState.h"
#include "memory/memory.h"



int main() {
    ProcessId = FindGamePID();
    if (!ProcessId) {
        std::cerr << "[-] Could not find game PID, is the game running?" << std::endl;
        return 0;
    }

    GameState gs = GameState(0x140000000);
    gs.tick();
}
