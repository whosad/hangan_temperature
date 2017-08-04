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
    void setScore(double* score){ _score = score; };

    // set sprite names
    void setSpriteNames(std::string bgName, std::string tileName){
        _bgSpriteName = bgName;
        _tileSpriteName = tileName;
        restartComponents();
    };

private:

    void update(float dt) override;

    // load background sprite
    void loadBackground();
    // scroll bg
    void scrollGameObjects();

    // load platforms
    void loadPlatforms();

    // setup schedule for obstacle spawn
    void scheduleObstacleSpawns(float dt);


    // load character
    void loadCharacter();

    // player physics (constant falling and collision?)
    void playerPhysics();

    // game over stuff
    void gameOverSequence();

    // update score 
    void updateScore(float dt);

    // restart ( or start new) components
    void restartComponents();



    // touch began event
    bool OnTouchBegan(cocos2d::Touch* t, cocos2d::Event* e);







    // Variables
public:


private:

    // visible size
    cocos2d::Size _visibleSize;

    // vectors of objects
    cocos2d::Vector<cocos2d::Sprite*> _backgrounds;
    cocos2d::Vector < cocos2d::Sprite*> _tilePlatforms;
    cocos2d::Vector < cocos2d::Sprite*> _obstacles;

    // game state pointer
    GAME_STATE* _gameState;

    // score pointer
    double* _score;

    // player character
    PlayerCharacter* _playerCharacter;

    // scrolling speed and modifier
    float _scrollSpeed;
    float _speedModifier;

    // fall speed
    float _fallSpeed;
    // acceleration
    const float _fallAcceleration = 0.45f;
    // maximum fall speed
    const float _maxFallSpeed = 13.f;
    // sign for reverse falling
    int _reverseFall;

    // position where player comes back to when hit
    float _defaultPlayerPosX;

    // obstacle spawn rate
    float _obstacleSpawnRate;

    std::string _bgSpriteName;
    std::string _tileSpriteName;

};

#endif //__GAME_LAYER_H__
