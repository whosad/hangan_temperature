#ifndef __GAME_LAYER_H__
#define __GAME_LAYER_H__

#include "cocos2d.h"
#include "GameState.h"

class PlayerCharacter;

// we are using node as layer
class GameLayer : public cocos2d::Node
{
    // Methods
public:

    // default init
    virtual bool init();    
    // implement the "static create()" method manually
    CREATE_FUNC(GameLayer);

    // holds the pointer of game state variable inside game scene
    void setGameState(GAME_STATE* gameState){ _gameState = gameState; };

private:

    void update(float dt) override;

    // load background sprite
    void loadBackground();
    // scroll bg
    void scrollGameObjects();

    // load platforms
    void loadPlatforms();

    // load obstacles

    // load character
    void loadCharacter();

    // player physics (constant falling and collision?)
    void playerPhysics();

    // Variables
public:


private:

    cocos2d::Size _visibleSize;

    float _scrollSpeed = 3.f;
    float _speedModifier = 1.f;
    cocos2d::Vector<cocos2d::Sprite*> _backgrounds;
    cocos2d::Vector < cocos2d::Sprite*> _tilePlatforms;

    GAME_STATE* _gameState;

    PlayerCharacter* _playerCharacter;

    float _fallSpeed = 0.f;
    const float _fallAcceleration = 0.075f;
    const float _maxFallSpeed = 5.f;
    float _reverseFall = -1.f;

};

#endif //__GAME_LAYER_H__
