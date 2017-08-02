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

	// setup schedule for obstacle spawn
	void scheduleObstacleSpawns(float dt);


    // load character
    void loadCharacter();

    // player physics (constant falling and collision?)
    void playerPhysics();

	// game over stuff
	void gameOverSequence();

	// restart ( or start new) components
	void restartComponents();


    // touch began event
    bool OnTouchBegan(cocos2d::Touch* t, cocos2d::Event* e);








    // Variables
public:


private:

    cocos2d::Size _visibleSize;


    cocos2d::Vector<cocos2d::Sprite*> _backgrounds;
	cocos2d::Vector < cocos2d::Sprite*> _tilePlatforms;
	cocos2d::Vector < cocos2d::Sprite*> _obstacles;

    GAME_STATE* _gameState;

    PlayerCharacter* _playerCharacter;

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

};

#endif //__GAME_LAYER_H__
