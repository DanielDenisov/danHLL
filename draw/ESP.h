#ifndef DANHLL_ESP_H
#define DANHLL_ESP_H
#include <cstring>

#include "Overlay.h"
#include "memory/memory.h"
#include "utils/localUtil.h"

inline Vector2 WorldToScreen(FMinimalViewInfo view, FVector world, int screenW, int screenH);
struct WeaponName {char str[32];};
WeaponName GetReadableWeaponName(uint8_t type);

inline void ESP(FMinimalViewInfo vm, std::vector<PlayerEnt> ents, uint8_t localPlayerTeam, std::vector<SpawnPoint> spawns) {
    Vector3 selfPos = {vm.location.x, vm.location.y, vm.location.z};

    constexpr float D2R = 3.14159265f / 180.f;
    const float pcy = cosf(vm.rotation.yaw * D2R);
    const float psy = sinf(vm.rotation.yaw * D2R);

    // ── Radar (transparent, centred, camera-relative) ─────────────────────────
    constexpr float RADAR_R     = 280.f;
    constexpr float RADAR_WORLD = 20000.f; // 200 m in UU
    const float RADAR_CX = config::SCREEN_W  * 0.5f;
    const float RADAR_CY = config::SCREEN_H * 0.5f;
    auto* dl = ImGui::GetBackgroundDrawList();
    // 200 m boundary ring + 100 m mid-ring
    dl->AddCircle(ImVec2(RADAR_CX, RADAR_CY), RADAR_R,        IM_COL32(255,255,255,35), 64, 1.f);
    dl->AddCircle(ImVec2(RADAR_CX, RADAR_CY), RADAR_R * 0.5f, IM_COL32(255,255,255,20), 64, 1.f);
    // Crosshair — forward is always "up" because enemy blips are rotated by cam yaw
    dl->AddLine(ImVec2(RADAR_CX, RADAR_CY - RADAR_R), ImVec2(RADAR_CX, RADAR_CY + RADAR_R), IM_COL32(255,255,255,22), 1.f);
    dl->AddLine(ImVec2(RADAR_CX - RADAR_R, RADAR_CY), ImVec2(RADAR_CX + RADAR_R, RADAR_CY), IM_COL32(255,255,255,22), 1.f);
    // Self dot
    dl->AddCircleFilled(ImVec2(RADAR_CX, RADAR_CY), 3.f, IM_COL32(80, 200, 80, 220));

    struct ThreatInfo { float angle; int dist; };
    ThreatInfo threats[32];
    int threatCount = 0;

    for (SpawnPoint sp : spawns) {
        auto color = IM_COL32(100, 100, 100, 100);
        int dist = sp.pos.Dist(selfPos)/100;

        if (sp.garrison) {
            color = IM_COL32(200, 30, 30, 120);
        } else { //outpost
            color = IM_COL32(35, 50, 200, 120);
        }

        Vector2 scrPos = WorldToScreen(vm, {sp.pos.x, sp.pos.y, sp.pos.z}, config::SCREEN_W, config::SCREEN_H);

        int rad{5};
        DrawCircleFilled(scrPos.x, scrPos.y, rad, color);

        char text[32] = "";
        sprintf(text, "%.0im", dist);
        DrawTextCentered(scrPos.x, scrPos.y /*+ rad + 5*/, IM_COL32(255, 255, 255, 200), text);
    }



    for (PlayerEnt ent : ents) {
        if (ent.team == localPlayerTeam) continue;
        if (ent.health < 1) continue;

        int dist = ent.pos.Dist(selfPos)/100;
        if (dist > config::maxPlayerDist) continue;

        // ── Aim detection (no WorldToScreen — works for all 360°) ────────────
        float lsp, lcp, lsy, lcy;
        {
            float pd = (float)(int8_t)ent.evi.pitch * (360.f / 256.f);
            lsp = sinf(pd * D2R); lcp = cosf(pd * D2R);
            lsy = sinf(ent.evi.rotation.yaw * D2R); lcy = cosf(ent.evi.rotation.yaw * D2R);
        }
        FVector entFwd = { lcp*lcy, lcp*lsy, lsp };
        FVector entEye = { ent.evi.location.x, ent.evi.location.y, ent.evi.location.z };
        // evi.location is root/feet; selfPos is camera/head — height delta alone
        // inflates 3D perpDist to ~1500 UU even for a direct shot, so check XY only.
        float dX     = selfPos.x - ent.pos.x;
        float dY     = selfPos.y - ent.pos.y;
        float along  = lcy * dX + lsy * dY;         // dot in XY: >0 means facing toward us
        float perp2d = fabsf(lcy * dY - lsy * dX);  // 2D cross: horizontal miss distance
        bool aimAtMe = along > 0.f && perp2d < 200.f; // 200 UU = 2 m horizontal miss radius

        // ── Threat ring (sub-200 m, any direction) ───────────────────────────
        if (aimAtMe && dist < 200 && threatCount < 32) {
            float dx       =  ent.pos.x - selfPos.x;
            float dy       =  ent.pos.y - selfPos.y;
            float rightDot = -dx * psy + dy * pcy;
            float fwdDot   =  dx * pcy + dy * psy;
            threats[threatCount++] = { atan2f(rightDot, fwdDot), dist };
        }

        // ── Radar blip + fire line (any direction) ───────────────────────────
        {
            float dx  = ent.pos.x - selfPos.x;
            float dy  = ent.pos.y - selfPos.y;
            float rdx = (-dx * psy + dy * pcy) * (RADAR_R / RADAR_WORLD);
            float rdy = -(dx * pcy + dy * psy) * (RADAR_R / RADAR_WORLD);

            float d     = sqrtf(rdx*rdx + rdy*rdy);
            bool atEdge = d > RADAR_R - 3.f;
            if (atEdge) { float s = (RADAR_R - 3.f) / d; rdx *= s; rdy *= s; }
            float px = RADAR_CX + rdx, py = RADAR_CY + rdy;

            ImU32 dotCol = aimAtMe ? IM_COL32(255, 60, 60, 240) : IM_COL32(220, 80, 80, 180);
            dl->AddCircleFilled(ImVec2(px, py), 3.f, dotCol);

            if (aimAtMe)
                dl->AddLine(ImVec2(RADAR_CX, RADAR_CY), ImVec2(px, py),
                            IM_COL32(255, 60, 60, 180), 1.5f);

            if (!atEdge) {
                float eFwdRdx =  (-lcp*lcy * psy + lcp*lsy * pcy);
                float eFwdRdy = -(lcp*lcy * pcy + lcp*lsy * psy);
                ImU32 fireCol = aimAtMe ? IM_COL32(255, 60, 60, 220) : IM_COL32(255, 180, 0, 130);
                dl->AddLine(ImVec2(px, py),
                            ImVec2(px + eFwdRdx * 12.f, py + eFwdRdy * 12.f),
                            fireCol, 1.5f);
            }
        }

        // ── Screen-space overlay (only when enemy is in front) ───────────────
        Vector2 feet = WorldToScreen(vm, {ent.pos.x, ent.pos.y, ent.pos.z+90.f}, config::SCREEN_W, config::SCREEN_H);
        Vector2 head = WorldToScreen(vm, {ent.pos.x, ent.pos.y, ent.pos.z-90.f}, config::SCREEN_W, config::SCREEN_H);
        if (feet.x == -1 || feet.y == -1 || head.x == -1 || head.y == -1) continue;

        char name[32];
        WeaponName result = GetReadableWeaponName(ent.weaponID);
        memcpy(name, result.str, 32);

        float h = feet.y - head.y;
        float w = h / 2.0f;
        if (config::isOnlyLine) {
            int lineX = head.x + w/2;
            int lineYHeight = (feet.y - head.y)*(ent.health/100);
            DrawLine(lineX, feet.y, lineX, head.y, IM_COL32(34, 189, 39, 70));
            DrawLine(lineX-w, feet.y, lineX-w, head.y, aimAtMe ? IM_COL32(207, 56, 56, 70) : IM_COL32(200, 200, 200, 70));
            if (!(100 - ent.health < 1))
                DrawLine(lineX, head.y-lineYHeight, lineX, feet.y, IM_COL32(207, 56, 56, 70));
        } else {
            DrawBox(head.x - w/2, head.y, w, h, IM_COL32(207, 56, 56, 255));
            int lineX = head.x - w/2 + 1 + 5-int(5*(dist/config::maxPlayerDist));
            int lineYHeight = (feet.y - head.y)*(ent.health/100);
            DrawLine(lineX, feet.y, lineX, feet.y, IM_COL32(34, 189, 39, 255));
            if (!(100 - ent.health < 1))
                DrawLine(lineX, head.y-lineYHeight, lineX, feet.y, IM_COL32(207, 56, 56, 255));
        }

        char dBuf[64];
        if (dist < config::maxPlayerWepDist)
            sprintf(dBuf, "%s - %.0im", name, dist);
        else
            sprintf(dBuf, "%.0im", dist);
        DrawTextCentered(head.x - w/2, head.y + 10, IM_COL32(255, 255, 255, 150), dBuf);

        // Laser line (screen-space only)
        {
            constexpr float LASER_LEN = 600.f;
            FVector laserTip = { entEye.x + entFwd.x * LASER_LEN,
                                 entEye.y + entFwd.y * LASER_LEN,
                                 entEye.z + entFwd.z * LASER_LEN };
            Vector2 scrEye = WorldToScreen(vm, entEye,   config::SCREEN_W, config::SCREEN_H);
            Vector2 scrTip = WorldToScreen(vm, laserTip, config::SCREEN_W, config::SCREEN_H);
            if (scrEye.x != -1 && scrTip.x != -1) {
                ImU32 laserCol = aimAtMe ? IM_COL32(255, 40, 40, 90) : IM_COL32(255, 200, 0, 90);
                DrawLine(scrEye.x, scrEye.y, scrTip.x, scrTip.y, laserCol);
            }
        }
    }

    // ── Aimed-at threat ring (sub-200 m) ─────────────────────────────────────
    if (threatCount > 0) {
        const float CX    = config::SCREEN_W * 0.5f;
        const float CY    = config::SCREEN_H * 0.5f;
        const float IND_R = RADAR_R + 28.f;  // just outside the 200 m radar ring
        const float AR    = 10.f;   // indicator radius

        for (int i = 0; i < threatCount; i++) {
            float ang = threats[i].angle; // 0=ahead, +π/2=right
            float ax  = CX + sinf(ang) * IND_R;
            float ay  = CY - cosf(ang) * IND_R;

            dl->AddCircleFilled(ImVec2(ax, ay), AR, IM_COL32(200, 30, 30, 190));
            dl->AddCircle(ImVec2(ax, ay), AR, IM_COL32(255, 100, 100, 220), 16, 1.5f);
            // Arrow pointing toward direction threat is coming from
            dl->AddLine(ImVec2(ax - sinf(ang) * (AR - 2.f), ay + cosf(ang) * (AR - 2.f)),
                        ImVec2(ax + sinf(ang) * (AR - 2.f), ay - cosf(ang) * (AR - 2.f)),
                        IM_COL32(255, 255, 255, 240), 2.f);

            char distStr[12];
            sprintf(distStr, "%dm", threats[i].dist);
            ImVec2 tsz = ImGui::CalcTextSize(distStr);
            dl->AddText(ImVec2(ax - tsz.x * 0.5f, ay + AR + 3.f),
                        IM_COL32(255, 80, 80, 220), distStr);
        }
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