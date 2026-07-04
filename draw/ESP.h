#ifndef DANHLL_ESP_H
#define DANHLL_ESP_H
#include <cstring>

#include "Overlay.h"
#include "utils/localUtil.h"

inline Vector2 WorldToScreen(FMinimalViewInfo view, FVector world, int screenW, int screenH);
struct WeaponName {char str[32];};
WeaponName GetReadableWeaponName(uint8_t type);

inline void ESP(FMinimalViewInfo vm, std::vector<PlayerEnt> ents, uint8_t localPlayerTeam) {
    Vector3 selfPos = {vm.location.x, vm.location.y, vm.location.z};
    for (PlayerEnt ent : ents) {
        if (ent.team == localPlayerTeam) continue; //skip same team
        if (ent.health < 1) continue; //skip dead
        //bad pos already filtered out

        int dist = ent.pos.Dist(selfPos)/100;

        if (dist > config::maxPlayerDist) continue;//out of range


        Vector2 feet = WorldToScreen(vm, {ent.pos.x, ent.pos.y, ent.pos.z+90.f}, config::SCREEN_W, config::SCREEN_H);
        Vector2 head = WorldToScreen(vm, {ent.pos.x, ent.pos.y, ent.pos.z-90.f}, config::SCREEN_W, config::SCREEN_H);

        if (feet.x == -1 || feet.y == -1 || head.x == -1 || head.y == -1) continue; //off-screen

        //get the name of a healed weapon.
        char name[32];
        WeaponName result = GetReadableWeaponName(ent.weaponID);
        memcpy(name, result.str, 32);

        float h = feet.y - head.y;
        float w = h / 2.0f;
        DrawBox(head.x - w/2, head.y, w, h, IM_COL32(207, 56, 56, 255));

        int lineX = head.x + w/2 + 1 + 5-int(5*(dist/config::maxPlayerDist)); //scale gap between box and hp line
        int lineYHeight = (feet.y - head.y)*(ent.health/100); //calc remaining health
        DrawLine(lineX, feet.y, lineX, feet.y-lineYHeight, IM_COL32(34, 189, 39, 255));
        if (!(100 - ent.health < 1)) { //not at full health
            DrawLine(lineX, feet.y-lineYHeight, lineX, head.y, IM_COL32(207, 56, 56, 255));
        }

        char dBuf[64];
        if (dist < config::maxPlayerWepDist) {
            sprintf(dBuf, "%s - %.0im", name, dist);
        } else {
            sprintf(dBuf, "%.0im", dist);
        }
        DrawTextCentered(head.x - w/2, head.y - 8, IM_COL32(255, 255, 255, 255), dBuf);
    }
}

inline Vector2 WorldToScreen(FMinimalViewInfo view, FVector world, int screenW, int screenH) {
    constexpr float DEG2RAD = 3.14159265f / 180.f;

    float sp = sinf(view.rotation.pitch * DEG2RAD), cp = cosf(view.rotation.pitch * DEG2RAD);
    float sy = sinf(view.rotation.yaw   * DEG2RAD), cy = cosf(view.rotation.yaw   * DEG2RAD);
    float sr = sinf(view.rotation.roll  * DEG2RAD), cr = cosf(view.rotation.roll  * DEG2RAD);

    FVector fwd   = {  cp*cy,               cp*sy,               sp    };
    FVector right = { -cr*sy - sr*sp*cy,    cr*cy - sr*sp*sy,    sr*cp };
    FVector up    = {  sr*sy - cr*sp*cy,   -sr*cy - cr*sp*sy,    cr*cp };

    FVector delta = { world.x - view.location.x,
                      world.y - view.location.y,
                      world.z - view.location.z };

    float localX = delta.x*right.x + delta.y*right.y + delta.z*right.z;
    float localY = delta.x*up.x    + delta.y*up.y    + delta.z*up.z;
    float localZ = delta.x*fwd.x   + delta.y*fwd.y   + delta.z*fwd.z;

    if (localZ < 0.01f)
        return { -1.f, -1.f };

    float scale = (screenW * 0.5f) / tanf(view.fov * DEG2RAD * 0.5f);

    return {
        screenW * 0.5f + (localX / localZ) * scale,
        screenH * 0.5f - (localY / localZ) * scale
    };
}



inline WeaponName GetReadableWeaponName(uint8_t type) {
    WeaponName output = {"Unknown"};

    const char* name = "Unknown";

    switch (type) {
        case 0:  name = "None"; break;
        case 1:  name = "Karabiner 98k"; break;
        case 2:  name = "M1 Garand"; break;
        case 3:  name = "MP 40"; break;
        case 4:  name = "M1A1 Thompson"; break;
        case 5:  name = "M24 Stielhandgranate"; break;
        case 6:  name = "Mk2 Grenade"; break;
        case 7:  name = "MG 42"; break;
        case 8:  name = "M1918A2 BAR"; break;
        case 9:  name = "M1 Carbine"; break;
        case 10: name = "M43 Stielhandgranate"; break;
        case 11: name = "Gewehr 43"; break;
        case 12: name = "M1903 Springfield"; break;
        case 13: name = "M1911 Colt"; break;
        case 14: name = "M1919 Browning"; break;
        case 15: name = "M1A1 SMG"; break;
        case 16: name = "M2 Carbine"; break;
        case 17: name = "Tellermine 43"; break;
        case 18: name = "S-Mine"; break;
        case 19: name = "Luger P08"; break;
        case 20: name = "Kar98k (Scoped)"; break;
        case 21: name = "StG 44"; break;
        case 22: name = "M1 Bazooka"; break;
        case 23: name = "Panzerschreck"; break;
        case 24: name = "Panzerfaust"; break;
        case 25: name = "Bandage"; break;
        case 26: name = "Morphine (US)"; break;
        case 27: name = "Morphine (GER)"; break;
        case 28: name = "M18 Smoke"; break;
        case 29: name = "Nb39 Smoke"; break;
        case 30: name = "Binoculars (US)"; break;
        case 31: name = "Binoculars (GER)"; break;
        case 32: name = "Supply Crate (US)"; break;
        case 33: name = "Supply Crate (GER)"; break;
        case 34: name = "Ammo Box (US)"; break;
        case 35: name = "Ammo Box (GER)"; break;
        case 36: name = "Walther P38"; break;
        case 37: name = "Watch (US)"; break;
        case 38: name = "Watch (GER)"; break;
        case 39: name = "Wrench"; break;
        case 40: name = "M2 Flamethrower"; break;
        case 41: name = "Flammenwerfer 41"; break;
        case 42: name = "Molotov Cocktail"; break;
        case 43: name = "Mosin Nagant M38"; break;
        case 44: name = "Mosin Nagant M1891"; break;
        case 45: name = "Mosin Nagant 91/30"; break;
        case 46: name = "SVT-40"; break;
        case 47: name = "Nagant M1895"; break;
        case 48: name = "PPSh-41"; break;
        case 49: name = "Hammer"; break;
        case 50: name = "M37 Satchel"; break;
        case 51: name = "3kg Satchel"; break;
        case 52: name = "Blowtorch (US)"; break;
        case 53: name = "Blowtorch (GER)"; break;
        case 54: name = "M3 Grease Gun"; break;
        case 55: name = "SVT-40 (Scoped)"; break;
        case 56: name = "Mosin 91/30 (Scoped)"; break;
        case 57: name = "PPSh-41 (Drum)"; break;
        case 58: name = "PTRS-41"; break;
        case 59: name = "DP-27"; break;
        case 60: name = "POMZ Mine"; break;
        case 61: name = "TM-35 Mine"; break;
        case 62: name = "RDG-42 Grenade"; break;
        case 63: name = "RDG-2 Smoke"; break;
        case 64: name = "Gammon Bomb"; break;
        case 65: name = "MG 34"; break;
        case 66: name = "Combat Knife"; break;
        case 67: name = "Tokarev TT-33"; break;
        case 68: name = "Watch (RU)"; break;
        case 69: name = "Blowtorch (RU)"; break;
        case 70: name = "Feldspaten (Shovel)"; break;
        case 71: name = "MPL-50 Shovel"; break;
        case 72: name = "Satchel (RU)"; break;
        case 73: name = "Ammo Box (RU)"; break;
        case 74: name = "Supply Crate (RU)"; break;
        case 75: name = "Binoculars (RU)"; break;
        case 76: name = "Medical Syringe"; break;
        case 77: name = "M1897 Trench Gun"; break;
        case 78: name = "FG 42"; break;
        case 79: name = "Flare Gun"; break;
        case 80: name = "Thompson (UK)"; break;
        case 81: name = "Thompson Drum (UK)"; break;
        case 82: name = "Thompson B"; break;
        case 83: name = "Thompson Drum B"; break;
        case 84: name = "Pattern 1914 Enfield"; break;
        case 85: name = "Lee-Enfield No.4 Mk1"; break;
        case 86: name = "No.4 Mk1 (Scoped)"; break;
        case 87: name = "Lee-Enfield Carbine"; break;
        case 88: name = "P14 Enfield (Scoped)"; break;
        case 89: name = "SMLE No.1 Mk3"; break;
        case 90: name = "Sten Gun"; break;
        case 91: name = "Sten Gun Mk V"; break;
        case 92: name = "Lanchester"; break;
        case 93: name = "Bren Gun"; break;
        case 94: name = "Lewis Gun"; break;
        case 95: name = "Webley Mk VI"; break;
        case 96: name = "Boys Anti-Tank Rifle"; break;
        case 97: name = "PIAT"; break;
        case 98: name = "Mills Bomb"; break;
        case 99: name = "No. 77 Smoke"; break;
        case 100: name = "GS Mk V Mine"; break;
        case 101: name = "Shrapnel Mine Mk II"; break;
        case 102: name = "Fairbairn-Sykes Knife"; break;
        case 103: name = "Binoculars (COM)"; break;
        case 104: name = "Supply Crate (COM)"; break;
        case 105: name = "Ammo Box (COM)"; break;
        case 106: name = "Blowtorch (COM)"; break;
        case 107: name = "Satchel (COM)"; break;
        case 108: name = "Watch (COM)"; break;
        case 109: name = "Morphine (COM)"; break;
        case 110: name = "Flamethrower (COM)"; break;
        case 111: name = "TNT Charge"; break;
        case 112: name = "Sten Gun (CAN)"; break;
        case 113: name = "Enfield No.2 Mk1"; break;
        case 114: name = "Browning Hi-Power"; break;
        default: name = "Unknown Weapon"; break;
    }

    // Safely copy the string into our stack-allocated return struct
    strncpy(output.str, name, sizeof(output.str) - 1);
    output.str[sizeof(output.str) - 1] = '\0'; // Ensure null termination

    return output;
}

#endif //DANHLL_ESP_H