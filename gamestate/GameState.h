#ifndef DANHLL_GAMESTATE_H
#define DANHLL_GAMESTATE_H
#include "config.h"
#include "utils/localUtil.h"

struct GSRet {
    std::vector<PlayerEnt> ents{};
    FMinimalViewInfo vm{};
    uint8_t teamID{1};
};

class GameState {
private:
    ptr gameBase{};

public:
    GameState(ptr gameBase = 0x140000000);
    GSRet tick();

private:
    ptr getUworld();


    std::vector<PlayerEnt> getEntities(ptr uworld);
    struct LPRet {
        uint8_t team{1};
        FMinimalViewInfo vm{};
    };
    LPRet getLPInfo(ptr uworld);


};


#endif //DANHLL_GAMESTATE_H