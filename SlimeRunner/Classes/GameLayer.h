#ifndef __GAME_LAYER_H__
#define __GAME_LAYER_H__

#include "cocos2d.h"
#include "GameState.h"

class PlayerCharacter;
class GameStageLoader;
class GameUILayer;

// we are using node as layer
class GameLayer : public cocos2d::Node
{
	friend GameUILayer;
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
	void setStageInfo(int stageNumber, std::string bgName, std::string tileName){
		_stageNumber = stageNumber;
		_bgSpriteName = bgName;
		_tileSpriteName = tileName;
		restartComponents();
	};

	void setGameUILayer(GameUILayer* gl){ _gameUILayer = gl; };

private:

	void update(float dt) override;

	// load background sprite
	void loadBackground();
	// scroll bg
	void scrollGameObjects();

	// load platforms
	void loadPlatforms();

	// setup schedule for obstacle spawn
	void scheduleBeeSpawns(float dt);
	void scheduleRandomGust(float dt);


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

	// start game
	void startGame();

	// spawn objects after certain pixels
	void obstacleSpawnFromData(float dt);

	// spawn object
	void spawnObstacle();

	// check if game is over
	void isGameOver();

	// wins game
	void winGameSequence();


	void scheduleLerpSpeedmod(float dt);


	void stopGameState();


	void checkCollision();





	///////////////// Variables
public:


private:

	// visible size
	cocos2d::Size _visibleSize;

	// vectors of objects
	cocos2d::Vector<cocos2d::Sprite*> _backgrounds;
	cocos2d::Vector < cocos2d::Sprite*> _tilePlatforms;
	cocos2d::Vector < cocos2d::Node*> _obstacles;
	// vector holds obstacle information
	std::vector<std::pair<float, OBSTACLE_TYPE>> _obstacleData;

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
	float _beeSpawnRate;
	float _gustSpawnRate;

	// string to be used to define sprite paths for stages
	std::string _bgSpriteName;
	std::string _tileSpriteName;

	// stage number
	int _stageNumber;

	// pixels passed since the start
	float _pixelsPassed;

	bool _wonGame;



	// holds background
	cocos2d::Node* _backgroundLayer = nullptr;

	// holds platforms
	cocos2d::Node* _platformLayer = nullptr;

	// holds obstacles
	cocos2d::Node* _obstacleLayer = nullptr;

	GameUILayer* _gameUILayer;

};

#endif //__GAME_LAYER_H__
