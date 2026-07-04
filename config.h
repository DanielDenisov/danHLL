#ifndef DANHLL_CONFIG_H
#define DANHLL_CONFIG_H

#include <cstdint>
#include <iostream>
#include <vector>

#define ptr uint64_t
#define pr inline ptr

inline constexpr bool isDebugMode = true;

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
    pr UWORLD = 0x46CCFA0;


    //UWorld -> GameState
    pr GAME_STATE = 0x130;

    //AGameStateBase->PlayerArray
    pr PLAYER_ARRAY = 0x238; //Of APlayerState, use PAWN to get pawn

    pr TEAM_ID = 0x49C; //not documented



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

    //UWorld->OwningGameInstance
    pr OWNING_GAME_INST = 0x188;
    //UGameInstance->LocalPlayers
    pr LOCAL_PLAYERS = 0x38;
    //UPlayer->PlayerController
    pr PlAYER_CONTROLLER = 0x30;
    //APlayerController->AcknowledgedPawn
    pr ACK_PAWN = 0x2A0;

    //APlayerController->PlayerCameraManagerClass
    pr CAM_MANAGER = 0x2C0;
    //APlayerCameraManager->CameraCachePrivate
    pr CAM_CACHE_PRIVATE = 0x1A60;


    // pr PRESISTENT_LEVEL = 0x30; //ts is like always at 0x30;
    //
    // pr ACTOR_ARRAY = 0x98;
    //
    // //AActor->RootComponent
    // pr ROOT_COMP = 0x130;
    // //USceneComponent->RelativeLocation
    // pr POS = 0x11c;
    //
    // //APawn->PlayerState
    // pr PLAYER_STATE = 0x240;
    // //

}


#endif //DANHLL_CONFIG_H