#include "GameState.h"

#include <chrono>
#include <thread>

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
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return {};
    }

    LPRet lpret = getLPInfo(uworld);
    if (lpret.vm.fov == 0) {
        std::cout << "[-] Failed to find view matrix" << std::endl;
        return {};
    }
    DBG{lpret.vm.Print();}
    // lpret.vm.rotation.roll /= 2;

    std::vector<SpawnPoint> spawns = getOutpostsGarrisonsPosition(uworld, lpret.team);

    return {ents, lpret.vm, lpret.team, spawns};
}

ptr GameState::getUworld() {
    ptr uworld = ReadMemory<ptr>(this->gameBase + off::UWORLD);

    return uworld;
}

bool isValidCoords(Vector3 pos) {
    if (1 < std::abs(pos.x) && std::abs(pos.x) < 50000 &&
        1 < std::abs(pos.y) && std::abs(pos.y) < 50000 &&
        1 < std::abs(pos.z) && std::abs(pos.z) < 50000) return true;
    return false;
}

void ScanForTeam(ptr pState, const char* label) {
    printf("=== %s (pState=0x%lX) ===\n", label, pState);
    for (int off = 0x480; off < 0x540; off += 1) {
        uint8_t val = ReadMemory<uint8_t>(pState + off);
        if (val == 1 || val == 2)
            printf("  0x%X = %d\n", off, val);
    }
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
    int PACount = ReadMemory<int>(gameState + off::PLAYER_ARRAY + sizeof(ptr));
    if (!playerArray || PACount == 0) {
        std::cout << "[-] Failed to find playerArray with count " << PACount << std::endl;
        return {};
    }
    DBG{std::cout << "[+] Found playerArray at 0x" << std::hex << playerArray << std::dec << " with count " << PACount << std::endl;}

    for (int a{}; a < PACount; a++) {
        ptr pState = ReadMemory<ptr>(playerArray + a * sizeof(ptr));
        if (!pState) continue;

        //scan, ask AI to find offset common to all
        // ScanForTeam(pState, std::to_string(a).c_str());

        PlayerEnt ent{};
        ent.pstate = pState;

        //Get Team
        ent.team = ReadMemory<uint8_t>(pState + off::TEAM_ID);

        ptr pawn = ReadMemory<ptr>(pState + off::PAWN);
        if (!pawn) continue;

        //Get Health
        ent.health = ReadMemory<float>(pawn + off::HEALTH);

        //Get Pos
        ptr rootComp = ReadMemory<ptr>(pawn + off::ROOT_COMP);
        ent.pos = ReadMemory<Vector3>(rootComp + off::POS);
        if (!isValidCoords(ent.pos)) continue; //skip bad coords

        //Get Weapon Type
        ptr currWeapon = ReadMemory<ptr>(pawn + off::CURR_WEAPON);
        ent.weaponID = ReadMemory<uint8_t>(currWeapon + off::TYPE);

        // --- PRINT TEST LOG --- put here (at bottom of file)

        eret.push_back(ent);
    }
    return eret;
}

std::vector<SpawnPoint> GameState::getOutpostsGarrisonsPosition(uint64_t uworld, uint8_t localPlayerTeam) {
    std::vector<SpawnPoint> spawns{};

    ptr plvl = ReadMemory<ptr>(uworld + off::PRESISTENT_LEVEL);
    ptr actorArray = ReadMemory<ptr>(plvl + off::PL_ACTOR_ARRAY);
    int count = ReadMemory<ptr>(plvl + off::PL_ACTOR_ARRAY + sizeof(ptr));

    for (int i{}; i < count; i++) {
        ptr actor = ReadMemory<ptr>(actorArray + (i * sizeof(ptr)));
        if (!actor) continue;

        if (ReadMemory<uint8_t>(actor + off::OWNING_TEAM) == localPlayerTeam) continue; //skips object (before vtable test) if its on same team

        ptr rootComp = ReadMemory<ptr>(actor + off::ROOT_COMP);
        if (!rootComp) continue;
        Vector3 pos = ReadMemory<Vector3>(rootComp + off::POS);

        ptr vtableAddr = ReadMemory<ptr>(actor);

        if (vtableAddr == off::VTABLE_OUTPOST) spawns.push_back({pos, false, true});
        if (vtableAddr == off::VTABLE_GARRISON) spawns.push_back({pos, true, false});
    }

    return spawns;
}

GameState::LPRet GameState::getLPInfo(uint64_t uworld) {
    LPRet lpret;
    ptr owngame = ReadMemory<ptr>(uworld + off::OWNING_GAME_INST);
    if (!owngame) {
        std::cout << "[-] Failed to find owning game inst" << std::endl;
        return{};
    }
    DBG{std::cout << "[+] Found owning game inst at 0x" << std::hex << owngame << std::dec << std::endl;}

    ptr localPlayers = ReadMemory<ptr>(owngame + off::LOCAL_PLAYERS);
    ptr localPlayer = ReadMemory<ptr>(localPlayers); //because only 1
    if (!localPlayer) {
        std::cout << "[-] Failed to find localPlayer" << std::endl;
        return{};
    }
    DBG{std::cout << "[+] Found localPlayer at 0x" << std::hex << localPlayer << std::dec << std::endl;}

    ptr playerController = ReadMemory<ptr>(localPlayer + off::PlAYER_CONTROLLER);
    if (!playerController) {
        std::cout << "[-] Failed to find playerController" << std::endl;
        return{};
    }
    DBG{std::cout << "[+] Found playerController at 0x" << std::hex << playerController << std::dec << std::endl;}

    //get pawn for team
    ptr pawn = ReadMemory<ptr>(playerController + off::ACK_PAWN);
    ptr playerState = ReadMemory<ptr>(pawn + off::PLAYER_STATE);
    lpret.team = ReadMemory<uint8_t>(playerState + off::TEAM_ID);

    ptr camManager = ReadMemory<ptr>(playerController + off::CAM_MANAGER);
    if (!camManager) {
        std::cout << "[-] Failed to find camManager" << std::endl;
        return{};
    }
    DBG{std::cout << "[+] Found camManager at 0x" << std::hex << camManager << std::dec << std::endl;}

    FCameraCacheEntry vmtemp = ReadMemory<FCameraCacheEntry>(camManager + off::CAM_CACHE_PRIVATE);
    lpret.vm = vmtemp.viewInfo;

    return lpret;
}

/** --- PRINT TEST LOG ---
        if (true){
            std::cout << "========================================" << std::endl;
            std::cout << "[+] Entity Base: 0x" << std::hex << pawn << std::dec << std::endl;
            std::cout << "    |-- Team ID:  " << (int)ent.team << std::endl; // Cast uint8_t to int to show number, not ASCII character
            std::cout << "    |-- Health:   " << ent.health << std::endl;
            std::cout << "    |-- Position: X: " << ent.pos.x << " | Y: " << ent.pos.y << " | Z: " << ent.pos.z << std::endl;

            if (currWeapon) {
                std::cout << "    |-- Weapon ID: " << (int)ent.weaponID << " (Weapon Ptr: 0x" << std::hex << currWeapon << std::dec << ")" << std::endl;
            } else {
                std::cout << "    |-- Weapon ID: [N/A] (No Active Weapon Equipped)" << std::endl;
            }
            std::cout << "========================================" << std::endl;
        }
 */