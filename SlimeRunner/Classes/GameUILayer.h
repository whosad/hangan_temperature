#ifndef __GAME_UI_LAYER_H__
#define __GAME_UI_LAYER_H__

#include "cocos2d.h"
#include "GameState.h"

class GameLayer;

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
	void setScore(double* score){ _score = score; };

	void setGameLayer(GameLayer* gl){ _gameLayer = gl; };

private:

	// update function
	void update(float dt) override;

	// update score
	void updateScore();

	// setup score label
	void setupScoreLabel();
	void updateHealth();
	void setupHealthBar();
	///////// Variables ////////////////////////////
public:


private:

	// visible size
	cocos2d::Size _visibleSize;

	// pointer to game scene variables
    GAME_STATE* _gameState;
	double* _score;
	float* _playerHealth;

	// score label
	cocos2d::Label* _scoreLabel;


	// game layer
	GameLayer* _gameLayer;


};

#endif //__GAME_UI_LAYER_H__
