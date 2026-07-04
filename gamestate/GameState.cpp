#include "GameState.h"

#include "memory/memory.h"

GameState::GameState(uintptr_t gameBase) {
    this->gameBase = gameBase;
}

GSRet GameState::tick() {
    ptr uworld = getUworld();
    if (!uworld) {
        std::cout << "[-] Uworld not found"  << std::endl;
        return {};
    }
    DBG{std::cout << "[+] Found uworld at " << std::hex << uworld << std::dec << std::endl;}

    std::vector<PlayerEnt> ents = getEntities(uworld);
    if (ents.empty()) {
        std::cout << "[-] 0 Entities found"  << std::endl;
        return {};
    }

    return {};
}

ptr GameState::getUworld() {
    ptr uworld = ReadMemory<ptr>(this->gameBase + off::UWORLD);

    return uworld;
}

std::vector<PlayerEnt> GameState::getEntities(uint64_t uworld) {
    std::vector<PlayerEnt> eret{};

    ptr gameState = ReadMemory<ptr>(uworld + off::GAME_STATE);
    if (!gameState) {
        std::cout << "[-] Failed to find gamestate" << std::endl;
        return{};
    }
    DBG{std::cout << "[+] Found gamestate at 0x" << std::hex << gameState << std::dec << std::endl;}

    ptr playerArray = ReadMemory<ptr>(gameState + off::PLAYER_ARRAY);
    ptr PACount = ReadMemory<ptr>(gameState + off::PLAYER_ARRAY + sizeof(ptr));
    if (!playerArray) {
        std::cout << "[-] Failed to find playerArray with count " << PACount << std::endl;
        return {};
    }
    DBG{std::cout << "[+] Found playerArray at 0x" << std::hex << playerArray << std::dec << " with count " << PACount << std::endl;}



    return eret;
}
