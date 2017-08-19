#ifndef __GAME_UI_LAYER_H__
#define __GAME_UI_LAYER_H__

#include "cocos2d.h"

#include "GameState.h"
#include "GameLayer.h"
#include "PlayerCharacter.h"

class GameScene;

// we are using node as layer
class GameUILayer : public cocos2d::Node
{
	friend GameLayer;

	// Methods
public:

	// default init
	virtual bool init();
	// implement the "static create()" method manually
	CREATE_FUNC(GameUILayer);

	// holds the pointer of game state variable inside game scene
	void setGameState(GAME_STATE* gameState){ _gameState = gameState; };
	void setScore(double* score){ _score = score; };

	// pointer to game layer
	void setGameLayer(GameLayer* gl){ _gameLayer = gl; };

	// updates health
	void updateHealth();

	void setPlayerHealth(int& ph){ _playerHealth = &ph; };

	void setInstruction(bool on){ _instructionSprite->setVisible(on); };

	void resetHealth();

private:

	// update function
	void update(float dt) override;

	// update score
	void updateScore();

	// setup score label
	void setupScoreLabel();
	// setup health bar
	void setupHealthBar();
	// setup skill gauge
	void setupGauge();


	// update gauge
	void updateGauge();


	// touch began event
       bool OnTouchBegan(cocos2d::Touch* t, cocos2d::Event* e);





	///////// Variables ////////////////////////////
public:


private:

	// visible size
	cocos2d::Size _visibleSize;

	// pointer to game scene variables
	GAME_STATE* _gameState;
	double* _score;
	int* _playerHealth;

	// score label
	cocos2d::Label* _scoreLabel;


	// game layer
	GameLayer* _gameLayer;

	// health icons vector
	cocos2d::Vector<cocos2d::Sprite*> _healthIcons;

	// setup instruction
	cocos2d::Sprite* _instructionSprite;

	// gauge bar
	cocos2d::Sprite* _gaugeBar;
	
	// skill icon
	cocos2d::Sprite* _skillIcon;
	// skill zoom action
	cocos2d::RepeatForever* _iconZoom;

};

#endif //__GAME_UI_LAYER_H__
