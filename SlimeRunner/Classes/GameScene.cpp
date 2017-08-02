#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    
     _visibleSize = Director::getInstance()->getVisibleSize();
//     Vec2 origin = Director::getInstance()->getVisibleOrigin();

     // game layer holds actual gameplay sprites and stuff
     _gameLayer = GameLayer::create();
     _gameLayer->setGameState(&_gameState);
     // ui layer only holds ui components
     _gameUILayer = GameUILayer::create();

     // in order
     this->addChild(_gameLayer, 0);
     this->addChild(_gameUILayer, 1);

     // ready go and start game


     // set update
     _gameLayer->scheduleUpdate();

    return true;
}
