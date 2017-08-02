#include "GameUILayer.h"
#include "SimpleAudioEngine.h"

#include <sstream>

USING_NS_CC;

// on "init" you need to initialize your instance
bool GameUILayer::init()
{
    //////////////////////////////
    // 1. super init first
    if(!Node::init())
    {
        return false;
    }

    CCLOG("Game UI Layer loaded");

	_visibleSize = Director::getInstance()->getVisibleSize();

	// setup score label
	_scoreLabel = Label::createWithTTF("Score: 0", "FONTS/kenpixel_blocks.ttf", 25.f, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::CENTER);
	_scoreLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	_scoreLabel->setPosition(_visibleSize.width * 0.05f, _visibleSize.height * .95f);
	_scoreLabel->setColor(Color3B::BLACK);
	this->addChild(_scoreLabel, 0);

    return true;
}

void GameUILayer::update(float dt)
{
	
	std::stringstream ss;

	ss << "Score: " << (int)*_score;

	_scoreLabel->setString(ss.str());


}
