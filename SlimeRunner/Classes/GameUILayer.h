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
	void setScore(double* score){ _score = score; };

private:

	// update function
	void update(float dt) override;



///////// Variables ////////////////////////////
public:


private:

	// visible size
	cocos2d::Size _visibleSize;

	// pointer to game scene variables
    GAME_STATE* _gameState;
	double* _score;

	// score label
	cocos2d::Label* _scoreLabel;



};

#endif //__GAME_UI_LAYER_H__
