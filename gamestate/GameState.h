#ifndef DANHLL_GAMESTATE_H
#define DANHLL_GAMESTATE_H
#include "config.h"
#include "utils/gameUtil.h"

struct GSRet {

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

};


#endif //DANHLL_GAMESTATE_H