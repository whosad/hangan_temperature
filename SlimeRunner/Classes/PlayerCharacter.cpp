#include "PlayerCharacter.h"


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
    this->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

    _playerHealth = 10;
    _isInvincible = false;
    _isMidAir = true;

    // set move animation
    // move animation has two frames
    auto spriteFrame0 = SpriteFrame::create("PNG/Enemies/slimePurple.png", this->getBoundingBox());
    auto spriteFrame1 = SpriteFrame::create("PNG/Enemies/slimePurple_move.png", this->getBoundingBox());
    spriteFrame0->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    spriteFrame1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

    auto moveAnimation = Animation::create();
    moveAnimation->setDelayPerUnit(0.15f);
    moveAnimation->addSpriteFrame(spriteFrame0);
    moveAnimation->addSpriteFrame(spriteFrame1);

    _moveAnimate = RepeatForever::create(Animate::create(moveAnimation));
	_moveAnimate->retain();

}

void PlayerCharacter::runBlink()
{
    auto blink = Blink::create(2.f, 14);
    auto moveBy = MoveBy::create(2.f, Vec2(-300.f, 0));

    // set is hit false

	
	this->runAction(Sequence::create(Spawn::create(blink, moveBy, nullptr), CallFuncN::create(CC_CALLBACK_0(PlayerCharacter::setHit, this, false)), nullptr));
}
