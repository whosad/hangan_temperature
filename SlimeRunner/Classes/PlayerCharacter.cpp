#include "PlayerCharacter.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

PlayerCharacter* PlayerCharacter::create()
{
    PlayerCharacter* pSprite = new PlayerCharacter();
    if(pSprite->initWithFile("PNG/Enemies/slimePurple.png")){
        pSprite->autorelease();
        pSprite->initOptions();
        return pSprite;
    }
    CC_SAFE_DELETE(pSprite);
    return nullptr;
}

void PlayerCharacter::initOptions()
{
    this->setFlippedX(true);

    _playerHealth = 10;
    _isInvincible = false;
    _isMidAir = true;
    _skillGauge = 0.f;
	_isEnlarged = false;

    // set move animation
    // move animation has two frames
    auto spriteFrame0 = SpriteFrame::create("PNG/Enemies/slimePurple.png", Rect(Vec2::ZERO, this->getContentSize()));
	auto spriteFrame1 = SpriteFrame::create("PNG/Enemies/slimePurple_move.png", Rect(Vec2::ZERO, this->getContentSize()));

	cocos2d::Texture2D::TexParams texParams = { GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };

	spriteFrame0->getTexture()->generateMipmap();
	spriteFrame0->getTexture()->setTexParameters(texParams);
	spriteFrame1->getTexture()->generateMipmap();
	spriteFrame1->getTexture()->setTexParameters(texParams);


    auto moveAnimation = Animation::create();
    moveAnimation->setDelayPerUnit(0.15f);
    moveAnimation->addSpriteFrame(spriteFrame0);
    moveAnimation->addSpriteFrame(spriteFrame1);

    _moveAnimate = RepeatForever::create(Animate::create(moveAnimation));
	_moveAnimate->retain();

	this->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);

}

void PlayerCharacter::runBlink()
{
    auto blink = Blink::create(2.f, 14);
    auto moveBy = MoveBy::create(1.33f, Vec2(-300.f, 0));

    // set is hit false

	
	this->runAction(Sequence::create(Spawn::create(blink, moveBy, nullptr), CallFuncN::create(CC_CALLBACK_0(PlayerCharacter::setHit, this, false)), nullptr));
}

void PlayerCharacter::skillEnlarge(float scale, bool enlarge)
{
	if (enlarge){
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("SFX/increase.wav");

	}
	else{
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("SFX/decrease.wav");

	}

	// get large
	auto scaleTo = ScaleTo::create(0.5, scale);
	scaleTo->setTag(51);
	this->runAction(scaleTo);
	this->stopActionByTag(31);
	_isEnlarged = enlarge;	
}