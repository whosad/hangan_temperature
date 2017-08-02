#include "GameScene.h"
#include "SimpleAudioEngine.h"

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

	 // 백 버튼으로 종료
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
		 }
		 };
	 this->_eventDispatcher->addEventListenerWithSceneGraphPriority(touchEvent, this);



    return true;
}
