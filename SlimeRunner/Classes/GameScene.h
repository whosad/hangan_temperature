#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "GameLayer.h"
#include "GameUILayer.h"
#include "GameState.h"

class TitleScene;

class GameScene : public cocos2d::Scene
{
    // Methods
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);

    // read stage data file and load obstacles
    bool setStage(int stage);

private:
    




    ///////////////////////////////// Variables
public:


private:
    cocos2d::Size _visibleSize;
    GameLayer* _gameLayer;
    GameUILayer* _gameUILayer;

    GAME_STATE _gameState = GAME_STATE::PAUSED;

    // score
    double _score = 0.0;
    
};

#endif //__GAME_SCENE_H__
