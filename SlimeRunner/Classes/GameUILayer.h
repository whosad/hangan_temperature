#ifndef __GAME_UI_LAYER_H__
#define __GAME_UI_LAYER_H__

#include "cocos2d.h"
#include "GameState.h"

// we are using node as layer
class GameUILayer : public cocos2d::Node
{
    // Methods
public:

    // default init
    virtual bool init();
    // implement the "static create()" method manually
    CREATE_FUNC(GameUILayer);

    // holds the pointer of game state variable inside game scene
    void setGameState(GAME_STATE* gameState){ _gameState = gameState; };

private:



    // Variables
public:


private:

    GAME_STATE* _gameState;

};

#endif //__GAME_UI_LAYER_H__
