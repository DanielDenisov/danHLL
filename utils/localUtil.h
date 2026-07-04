#ifndef DANHLL_LOCALUTIL_H
#define DANHLL_LOCALUTIL_H

#include "config.h"
#include "gameUtil.h"

struct PlayerEnt {
    ptr pstate{};
    uint8_t team{};
    float health{};
    Vector3 pos{};
    uint8_t weaponID{};

};





#endif //DANHLL_LOCALUTIL_H