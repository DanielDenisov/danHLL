#ifndef DANHLL_LOCALUTIL_H
#define DANHLL_LOCALUTIL_H

#include "config.h"
#include "gameUtil.h"

struct EnemyViewInfo {
    Vector3 location{};
    FRotator rotation{};
    Vector3 scale{};
    uint8_t pitch{};
};

struct PlayerEnt {
    ptr pstate{};
    uint8_t team{};
    float health{};
    Vector3 pos{};
    uint8_t weaponID{};
    EnemyViewInfo evi{};
};

struct SpawnPoint {
    Vector3 pos;
    bool garrison{};
    bool outpost{};
};





#endif //DANHLL_LOCALUTIL_H