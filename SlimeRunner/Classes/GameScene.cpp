#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "GameLayer.h"
#include "GameUILayer.h"


#include "TitleScene.h"

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
    
	 CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(.5f);

	 // ui layer only holds ui components
     _gameUILayer = GameUILayer::create();
	 _gameUILayer->setGameState(&_gameState);
	 _gameUILayer->setScore(&_score);

     // game layer holds actual gameplay sprites and stuff
     _gameLayer = GameLayer::create();
     _gameLayer->setGameState(&_gameState);
     _gameLayer->setScore(&_score);
     

	 // mutually pointing
	 _gameUILayer->setGameLayer(_gameLayer);
	 _gameLayer->setGameUILayer(_gameUILayer);

     // in order
     this->addChild(_gameLayer, 0);
     this->addChild(_gameUILayer, 1);

     // ready go and start game


     // set update
	 _gameLayer->scheduleUpdate();
	 _gameUILayer->scheduleUpdate();


	 // back button listener

	 auto touchEvent = EventListenerKeyboard::create();
	 touchEvent->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* e){
		 // quit application upon pressing back button
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		 // back button for android
		 if (keyCode == EventKeyboard::KeyCode::KEY_BACK){
#else
		 // backspace for windows debugging
		 if (keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE){
#endif
			 _gameUILayer->pause();
			 _gameLayer->pause();
			 Director::getInstance()->replaceScene(TransitionFade::create(0.3f, TitleScene::create(), Color3B::BLACK));
			 CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		 }
		 };
	 this->_eventDispatcher->addEventListenerWithSceneGraphPriority(touchEvent, this);



    return true;
}

bool GameScene::setStage(int stage)
{
    switch(stage)
    {
        case 0:
            _bgSpriteName = "colored_grass.png";
            _tileSpriteName = "grass";
            break;
        case 1:
            _bgSpriteName = "colored_desert.png";
            _tileSpriteName = "sand";
            break;
        case 2:
            _bgSpriteName = "colored_shroom.png";
            _tileSpriteName = "dirt";
            break;
    }

    _gameLayer->setStageInfo(stage, _bgSpriteName, _tileSpriteName);

    return true;
}

