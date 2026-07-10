#ifndef DANHLL_CONFIG_H
#define DANHLL_CONFIG_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <cmath>

namespace config {
    const inline int SCREEN_H = 1405;
    const inline int SCREEN_W = 2560;

    const inline double maxPlayerDist = 400; //meters
    const inline double maxPlayerWepDist = 40;

    const inline bool isOnlyLine = true;

    inline const auto OVERLAY_NAME = "Better Discord Overlay";
}

#define ptr uint64_t
#define pr inline ptr

inline constexpr bool isDebugMode = false;

#define DBG if(!isDebugMode) {} else
struct asHex {
    const int& value;
    friend std::ostream& operator<<(std::ostream& os, const asHex& ah) {
        return os << std::hex << ah.value << std::dec;
    }
};



namespace off {
    //ida dissasembler view.
    //48 8B 0D ? ? ? ? 48 85 C9 74 ? E8 ? ? ? ? 48 8B C8 (The conditional engine context load)
    //after scan, look at first function with qword_..., double click, and make sure its cross-references like 1k times
    pr UWORLD = 0x46ccfa0;


    //Uworld->PersistentLevel
    pr PRESISTENT_LEVEL = 0x30;
    //Either 0x98 to 0xA0
    pr PL_ACTOR_ARRAY = 0x98;
    //ADynamicSpawn->m_OwningTeam
    pr OWNING_TEAM = 0x2a8; //type ETeam, witch is actually a uint8


    //UWorld -> GameState
    pr GAME_STATE = 0x130;

    //AGameStateBase->PlayerArray
    pr PLAYER_ARRAY = 0x238; //Of APlayerState, use PAWN to get pawn

    pr TEAM_ID = 0x4B4; //not documented


    //APlayerState->PrivatePawn
    pr PAWN = 0x280;
    //AShooterCharacter->Health
    pr HEALTH = 0xa84; //float
    //AShooterCharacter->CurrentWeapon
    pr CURR_WEAPON = 0x5F0; //pointer
    //AShooterWeapon->Type
    pr TYPE = 0x300; //uint8
    //list of type and name under this: EWeaponType

    // //AActor->RootComponent
    pr ROOT_COMP = 0x130;
    //USceneComponent->RelativeLocation
    pr POS = 0x11c;
    //USceneComponent->RelativeRotation
    pr ROTATION = 0x128;
    //USceneComponent->RelativeScale3D
    pr SCALE = 0x134;
    //APawn->RemoteViewPitch
    pr PITCH = 0x232;

    //UWorld->OwningGameInstance
    pr OWNING_GAME_INST = 0x188;
    //UGameInstance->LocalPlayers
    pr LOCAL_PLAYERS = 0x38;
    //UPlayer->PlayerController
    pr PlAYER_CONTROLLER = 0x30;
    //APlayerController->AcknowledgedPawn
    pr ACK_PAWN = 0x2A0;
    //APawn->PlayerState
    pr PLAYER_STATE = 0x240;

    //APlayerController->PlayerCameraManager
    pr CAM_MANAGER = 0x2b8;
    //APlayerCameraManager->CameraCachePrivate
    pr CAM_CACHE_PRIVATE = 0x1A60;


    pr VTABLE_OUTPOST = 0x1432F9C20;
    pr VTABLE_GARRISON = 0x1432F7D60;

}


#endif //DANHLL_CONFIG_H