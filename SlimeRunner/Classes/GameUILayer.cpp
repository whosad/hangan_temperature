#include "GameUILayer.h"
#include "SimpleAudioEngine.h"

#include "GameScene.h"
#include "GameLayer.h"
#include "TitleScene.h"

USING_NS_CC;

// on "init" you need to initialize your instance
bool GameUILayer::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Node::init())
	{
		return false;
	}

	CCLOG("Game UI Layer loaded");

	_visibleSize = Director::getInstance()->getVisibleSize();


	// add touch listeners
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameUILayer::OnTouchBegan, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);


	setupScoreLabel();
	setupHealthBar();
	setupGauge();

	_instructionSprite = Sprite::create("PNG/instruction.png");
	// not recommended but for now..
	_instructionSprite->setPosition(_visibleSize.width * .5f - 379.f, _visibleSize.height * .5f + 88.f - 148.f);
	this->addChild(_instructionSprite);


	return true;
}

void GameUILayer::update(float dt)
{


	if (*_gameState == GAME_STATE::PLAYING){
		updateScore();
		updateGauge();

	}
	else 	if (*_gameState == GAME_STATE::OVER){
		

	}
}

void GameUILayer::updateScore()
{
	std::stringstream ss;

	ss << (int)*_score;

	_scoreLabel->setString(ss.str());

}

void GameUILayer::setupScoreLabel()
{
	//_scoreLabel = Label::createWithTTF("Score: 0", "FONTS/kenpixel_blocks.ttf", 60.f, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::CENTER);
	//    _scoreLabel->setColor(Color3B::BLACK);
	_scoreLabel = Label::createWithCharMap("PNG/HUD/hudNumbers.png", 64, 128, 48);
	_scoreLabel->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
	_scoreLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	_scoreLabel->setPosition(_visibleSize.width * 0.03f, _visibleSize.height + 10.f);
	this->addChild(_scoreLabel, 0);


	// not a right place but..
	auto popup = Sprite::create("PNG/Ending/end_popup.png");
	popup->setPosition(_visibleSize.width / 2, _visibleSize.height / 2);
	popup->setName("endPopup");
	popup->setVisible(false);
	this->addChild(popup);
}

void GameUILayer::updateHealth()
{
	CCLOG("player health on ui: %d", _gameLayer->_playerCharacter->getHealth());

	if (*_playerHealth % 2 == 0){
		_healthIcons.at(*_playerHealth / 2)->setSpriteFrame(Sprite::create("PNG/HUD/hudHeart_empty.png")->getSpriteFrame());

	}
	else{
		_healthIcons.at(*_playerHealth / 2)->setSpriteFrame(Sprite::create("PNG/HUD/hudHeart_half.png")->getSpriteFrame());
	}

	// shaky heart? cam?
	auto shake1 = MoveBy::create(.05f, Vec2(random(-3, 3), random(-3, 3)));
	auto shake2 = MoveBy::create(.05f, Vec2(random(-3, 3), random(-3, 3)));
	auto shake3 = MoveBy::create(.05f, Vec2(random(-3, 3), random(-3, 3)));

	auto sequence = Sequence::create(shake1, shake1->reverse(), shake2, shake2->reverse(), shake3, shake3->reverse(), nullptr);

	//_healthIcons.at(*_playerHealth / 2)->runAction(sequence);

	GameScene* scene = (GameScene*)this->getParent();
	scene->getDefaultCamera()->runAction(sequence);
}

void GameUILayer::setupHealthBar()
{
	// player has 100 hp = 5 hearts
	auto heart = Sprite::create("PNG/HUD/hudHeart_full.png");

	for (int i = 0; i < 3; i++){
		auto heartClone = Sprite::createWithSpriteFrame(heart->getSpriteFrame());
		heartClone->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
              heartClone->setScale(.75f);
              heartClone->setPosition(_visibleSize.width - heartClone->getContentSize().width * .6f * (i), _visibleSize.height);
		_healthIcons.pushBack(heartClone);
		this->addChild(heartClone);
	}
}

void GameUILayer::resetHealth()
{
	for (auto spr : _healthIcons){
		spr->setSpriteFrame(Sprite::create("PNG/HUD/hudHeart_full.png")->getSpriteFrame());
	}
}

void GameUILayer::updateGauge()
{
	auto val = _gameLayer->_playerCharacter->getGauge();
	_gaugeBar->getChildByName<ProgressTimer*>("bar")->setPercentage(val);

	// skill uses 1/2 of gauge
	if (val > 50.f){
		if (_skillIcon->getNumberOfRunningActions() <= 0)
			_skillIcon->runAction(_iconZoom);
	}
	else{
		if (_skillIcon->getNumberOfRunningActions() > 0)
			_skillIcon->stopAllActions();
	}
}

void GameUILayer::setupGauge()
{
	// Sprite node for background
	_gaugeBar = Sprite::create("PNG/UI_Pack/yellow_button13.png");
	_gaugeBar->setPosition(_visibleSize.width * .1f, _visibleSize.height * .03f);
	_gaugeBar->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	// progess bar?
	auto gaugeBar = ProgressTimer::create(Sprite::create("PNG/UI_Pack/yellow_button_fill.png"));
	gaugeBar->setType(ProgressTimer::Type::BAR);
	gaugeBar->setBarChangeRate(Vec2(1.f, 0.f));
	gaugeBar->setMidpoint(Vec2(0.f, .5f));
	gaugeBar->setPercentage(30.f);
	gaugeBar->setName("bar");
	gaugeBar->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	_gaugeBar->addChild(gaugeBar);

	// icon
	_skillIcon = Sprite::create("PNG/skillIcon.png");
	_skillIcon->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
	_skillIcon->setPosition(0.f, _gaugeBar->getContentSize().height * .5f + 2.f);
	_skillIcon->setName("icon");
	_gaugeBar->addChild(_skillIcon);

	this->addChild(_gaugeBar);

	// add touch listeners
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameUILayer::OnTouchBegan, this);
	touchListener->setSwallowTouches(true);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, _gaugeBar);

	auto scaleTo = ScaleTo::create(.5f, 1.2f);
	auto scaleToRev = ScaleTo::create(.5f, 1.f);
	auto seq = Sequence::create(scaleTo, scaleToRev, nullptr);
	_iconZoom = RepeatForever::create(seq);
	_iconZoom->retain();

	// middle indicator 
	auto midIndicator = Sprite::create("PNG/UI_Pack/blue_sliderUp.png");
	midIndicator->setFlippedY(true);
	midIndicator->setScale(.3f);
	midIndicator->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	_gaugeBar->addChild(midIndicator);
	midIndicator->setPosition(_gaugeBar->getContentSize().width * .5f, _gaugeBar->getContentSize().height - 10.f);
}


bool GameUILayer::OnTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	// touch on button
	if (event->getCurrentTarget() == _gaugeBar ){
		if (!_gameLayer->_wonGame){

			// find out which one is touched and proceed to selected stage
			auto target = (ProgressTimer*)(event->getCurrentTarget());
			auto icon = (Sprite*)(event->getCurrentTarget()->getChildByName("icon"));

			// gauge itself
			auto locInNode = target->convertToNodeSpace(touch->getLocation());
			auto size = target->getContentSize();
			auto rect = Rect(0, 0, size.width, size.height);
			// skill icon
			auto locInIcon = icon->convertToNodeSpace(touch->getLocation());
			auto iconSize = icon->getContentSize();
			auto iconRect = Rect(0, 0, iconSize.width, iconSize.height);

			// both can activate skill
			if (rect.containsPoint(locInNode) || iconRect.containsPoint(locInIcon)){
				CCLOG("button touch");
				// cancel skill if already active
				if (_gameLayer->_playerCharacter->isEnlarged()){
					_gameLayer->_playerCharacter->skillEnlarge(1.f, false);
				}
				else{
					// upon activation, lose some and constant decreament and RESETS speed modifier 
					if (_gameLayer->_playerCharacter->getGauge() >= 50.f){
						_gameLayer->_playerCharacter->increaseGauge(-13.f);
						_gameLayer->_playerCharacter->skillEnlarge();
						_gameLayer->_speedModifier = 1.f;
					}
				}

				return true;
			}
		}
		return false;
	}
	// touch on screen
	else{

		switch (*_gameState){
			case GAME_STATE::PLAYING:
				if (_gameLayer->_playerCharacter->isEnlarged() || _gameLayer->_playerCharacter->getActionByTag(51)){
					break;
				}
				if (!(_gameLayer->_playerCharacter->isMidAir())){
					// jump
					// reverse fall direction
					_gameLayer->_reverseFall *= -1;
					// jump player 
					_gameLayer->_fallSpeed = 4.f;
					_gameLayer->_playerCharacter->setPositionY(_gameLayer->_playerCharacter->getPositionY() + _gameLayer->_reverseFall * _gameLayer->_fallSpeed);

					// flip sprite
					_gameLayer->_playerCharacter->setFlippedY(!_gameLayer->_playerCharacter->isFlippedY());

					// change anchor
					if (_gameLayer->_playerCharacter->isFlippedY()){
						_gameLayer->_playerCharacter->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
						_gameLayer->_playerCharacter->setPositionY(_gameLayer->_playerCharacter->getPositionY() + _gameLayer->_playerCharacter->getContentSize().height);
					}
					else{
						_gameLayer->_playerCharacter->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
						_gameLayer->_playerCharacter->setPositionY(_gameLayer->_playerCharacter->getPositionY() - _gameLayer->_playerCharacter->getContentSize().height);
					}

				}
				break;

			case GAME_STATE::PAUSED:
				// start game
				_gameLayer->startGame();
				_gameLayer->_gameUILayer->setInstruction(false);

				// stage 1 spawns nothing, 2 spawns bee, 3 spawns both bee and gust

				if (_gameLayer->_stageNumber >= 2){
					_gameLayer->schedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleBeeSpawns), _gameLayer->_beeSpawnRate);
				}
				if (_gameLayer->_stageNumber >= 1){
					_gameLayer->schedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleRandomGust), _gameLayer->_gustSpawnRate);
				}

				break;

			case GAME_STATE::OVER:

				// restart button touched
				auto locInView = touch->getLocationInView();
				auto restartRect = Rect(_visibleSize.width / 2 - 250 + 300, _visibleSize.height / 2 - 150 + 100, 100, 100); 
				auto endRect = Rect(_visibleSize.width / 2 - 250 + 90, _visibleSize.height / 2 - 150 + 100, 100, 100);
				if (restartRect.containsPoint(locInView)){

					CCLOG("Game over and touched");

					// clean up and reset componets
					_gameLayer->removeAllChildrenWithCleanup(true);

					_gameLayer->_backgrounds.clear();
					_gameLayer->_tilePlatforms.clear();
					_gameLayer->_obstacles.clear();

					_gameLayer->restartComponents();
					_scoreLabel->setString("0");
					_gaugeBar->getChildByName<ProgressTimer*>("bar")->setPercentage(30.f);
					resetHealth();

					*_gameState = GAME_STATE::PAUSED;
				}
				else if (endRect.containsPoint(locInView)){
					this->pause();
					_gameLayer->pause();
					Director::getInstance()->replaceScene(TransitionFade::create(0.3f, TitleScene::create(), Color3B::BLACK));
					CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
				}
				break;

		}


		return false;
	}
}

