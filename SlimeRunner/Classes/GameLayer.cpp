#include "GameLayer.h"
#include "SimpleAudioEngine.h"
#include "PlayerCharacter.h"

USING_NS_CC;

// on "init" you need to initialize your instance
bool GameLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Node::init() )
    {
        return false;
    }
    
    CCLOG("Game Layer loaded");

    // set visibe size
    _visibleSize = Director::getInstance()->getVisibleSize();

    loadBackground();
    
    loadPlatforms();

    loadCharacter();


    // schedule game loop
    this->scheduleUpdate();
     
    return true;
}

void GameLayer::update(float dt){
    
    scrollGameObjects();

    playerPhysics();
}

void GameLayer::loadBackground()
{
    // load initial image
    _backgrounds.pushBack(Sprite::create("PNG/Backgrounds/colored_grass.png"));
    _backgrounds.front()->setAnchorPoint(Vec2::ZERO);
    _backgrounds.front()->setPosition(0.f, 0.f);
    this->addChild(_backgrounds.front());

    // set number of duplicated background image using width and screen width
    // starts at 1 since we already have one at the beginning
    auto bgSize = ceil(_visibleSize.width / _backgrounds.front()->getContentSize().width) + 1;
    for(int i = 1; i < bgSize; i++){
        // duplicate sprite
        _backgrounds.pushBack(Sprite::createWithSpriteFrame(_backgrounds.front()->getSpriteFrame()));
        // se anchor point to 0,0
        _backgrounds.back()->setAnchorPoint(Vec2::ZERO);
        // set initial positions

        auto w = _backgrounds.front()->getContentSize().width;
        auto tobeset = _backgrounds.front()->getContentSize().width * i;
        CCLOG("width is %f and pos is %f", w, tobeset);
        _backgrounds.back()->setPosition(_backgrounds.front()->getContentSize().width * i, 0.f);
        // add to scene
        this->addChild(_backgrounds.back());
    }
}

void GameLayer::scrollGameObjects()
{
    // background sprites
    for(auto bg : _backgrounds){
        // move sprites
        bg->setPositionX(bg->getPositionX() - _scrollSpeed * _speedModifier * .6f);
        // if the position is completely off the screen, relocate it to the end of the other sprite
        if(bg->getPositionX() <= -bg->getContentSize().width){
            bg->setPositionX(bg->getPositionX() + bg->getContentSize().width * _backgrounds.size()); // width * number of sprites
        }
    }
    
    // platforms
    for(auto it = _tilePlatforms.begin(); it != _tilePlatforms.end();){
        (*it)->setPositionX((*it)->getPositionX() - _scrollSpeed * _speedModifier);
        if((*it)->getPositionX() < -(*it)->getContentSize().width * 2.f){
            this->removeChild(*it);
            it = _tilePlatforms.erase(it);
        }
        else{
            ++it;
        }
    }


}

void GameLayer::loadPlatforms(){
    // 처음엔 미들 플랫폼으로 위 아래 다 깔아버려
    auto midTile = Sprite::create("PNG/Ground/Grass/grassHalf_mid.png");
    int numOfPlatforms = ceil(_visibleSize.width / midTile->getContentSize().width) + 1;

    for(int i = 0; i < numOfPlatforms; i++){

        // create top and bottom tiles
        auto bottom = Sprite::createWithSpriteFrame(midTile->getSpriteFrame());
        bottom->setAnchorPoint(Vec2::ZERO);
        bottom->setPositionX(i * midTile->getContentSize().width);
        bottom->setPositionY(-midTile->getContentSize().height * .5f);

        auto top = Sprite::createWithSpriteFrame(midTile->getSpriteFrame());
        top->setAnchorPoint(Vec2::ZERO);
        top->setPositionX(i * midTile->getContentSize().width);
        top->setPositionY(_visibleSize.height - midTile->getContentSize().height * .5f);
        top->setFlippedY(true);

        // add to node
        this->addChild(top, 0);
        this->addChild(bottom, 0);

        // add tiles to vector for convinence
        _tilePlatforms.pushBack(top);
        _tilePlatforms.pushBack(bottom);
    }
    
    
}

void GameLayer::loadCharacter()
{
    _playerCharacter = PlayerCharacter::create();

    this->addChild(_playerCharacter, 0);
    _playerCharacter->setPosition(200, 200);

}

void GameLayer::playerPhysics()
{

    // maximum falling speed is defined in the header
    _fallSpeed = MIN(_fallSpeed + _fallAcceleration, _maxFallSpeed);

    if(_playerCharacter->isMidAir()){
        _playerCharacter->setPositionY(_playerCharacter->getPositionY() +( _reverseFall * _fallSpeed));
        
    }
    // check collision
    for(auto tile : _tilePlatforms){
        if(tile->getBoundingBox().intersectsRect(_playerCharacter->getBoundingBox())){
            _playerCharacter->setMidAir(false);
            _playerCharacter->setPositionY(tile->getPositionY() + tile->getContentSize().height);
            _fallSpeed = 0.f;
            break;
        }
        else{
            _playerCharacter->setMidAir(true);
        }
    }

 
}
