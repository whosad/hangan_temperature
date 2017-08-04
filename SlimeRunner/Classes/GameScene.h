#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "GameState.h"


class TitleScene;
class GameLayer;
class GameUILayer;

class GameScene : public cocos2d::Scene
{
    // Methods
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);

    /* 
    Read stage data file and load obstacles.
    @description Parses from file and load stage information.    
    */
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
    
    std::string _bgSpriteName;
    std::string _tileSpriteName;

};

#endif //__GAME_SCENE_H__
