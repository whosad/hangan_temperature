#include "GameUILayer.h"
#include "SimpleAudioEngine.h"

#include "GameLayer.h"

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

	setupScoreLabel();
	setupHealthBar();


    return true;
}

void GameUILayer::update(float dt)
{
	
	updateScore();

}

void GameUILayer::updateScore()
{
	std::stringstream ss;

	ss << "Score: " << (int)*_score;

	_scoreLabel->setString(ss.str());
}

void GameUILayer::setupScoreLabel()
{
	_scoreLabel = Label::createWithTTF("Score: 0", "FONTS/kenpixel_blocks.ttf", 60.f, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::CENTER);
	_scoreLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	_scoreLabel->setPosition(_visibleSize.width * 0.05f, _visibleSize.height * .98f);
	_scoreLabel->setColor(Color3B::BLACK);
	this->addChild(_scoreLabel, 0);
}

void GameUILayer::updateHealth()
{
	CCLOG("player health on ui: %d", _gameLayer->getPlayerHealth());

       // shaky heart?
       auto shake1 = MoveBy::create(.05f, Vec2(random(-1.5, 1.5), random(-1.5, 1.5)));
       auto shake2 = MoveBy::create(.05f, Vec2(random(-1.5, 1.5), random(-1.5, 1.5)));
       auto shake3 = MoveBy::create(.05f, Vec2(random(-1.5, 1.5), random(-1.5, 1.5)));

       auto sequence = Sequence::create(shake1,  shake1->reverse(), shake2, shake2->reverse(), shake3, shake3->reverse(), nullptr);


       if(*_playerHealth % 2 == 0){
           _healthIcons.at(*_playerHealth / 2)->setSpriteFrame(Sprite::create("PNG/HUD/hudHeart_empty.png")->getSpriteFrame());

       }
       else{
           _healthIcons.at(*_playerHealth / 2)->setSpriteFrame(Sprite::create("PNG/HUD/hudHeart_half.png")->getSpriteFrame());
       }
       _healthIcons.at(*_playerHealth / 2)->runAction(sequence);

}

void GameUILayer::setupHealthBar()
{
	// player has 100 hp = 5 hearts
    auto heart = Sprite::create("PNG/HUD/hudHeart_full.png");
    
    for(int i = 0; i < 5; i++){
        auto heartClone = Sprite::createWithSpriteFrame(heart->getSpriteFrame());
        heartClone->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
        heartClone->setPosition(_visibleSize.width - heartClone->getContentSize().width * .8f * (i)   , _visibleSize.height * .98f);
        _healthIcons.pushBack(heartClone);
        this->addChild(heartClone);
    }
}
