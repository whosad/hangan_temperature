#include "GameLayer.h"
#include "SimpleAudioEngine.h"
#include "PlayerCharacter.h"
#include "GameStageLoader.h"


USING_NS_CC;

// on "init" you need to initialize your instance
bool GameLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if(!Node::init())
    {
        return false;
    }

    CCLOG("Game Layer loaded");

    // set visibe size
    _visibleSize = Director::getInstance()->getVisibleSize();


    // set obstacle spawn schedule
    	this->schedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleObstacleSpawns), 1);

    // add touch listeners
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(GameLayer::OnTouchBegan, this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);


    /************************************************************************/
#ifdef _DEBUG	
    auto keyListener = EventListenerKeyboard::create();
    auto pointerToMod = &_speedModifier;
    keyListener->onKeyPressed = [pointerToMod](EventKeyboard::KeyCode keyCode, Event* event){
        if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
            *pointerToMod = 5.f;
    };
    keyListener->onKeyReleased = [pointerToMod](EventKeyboard::KeyCode keyCode, Event* event){
        if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
            *pointerToMod = 1.f;
    };

    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);


#endif
    /************************************************************************/



    return true;
}

void GameLayer::update(float dt){

    switch(*_gameState)
    {
        case GAME_STATE::PLAYING:
            scrollGameObjects();

            playerPhysics();

            updateScore(dt);
            break;
        case GAME_STATE::PAUSED:

            break;
    }

}

void GameLayer::loadBackground()
{
    // load initial image
    std::stringstream ss;
    ss << "PNG/Backgrounds/" << _bgSpriteName;
    _backgrounds.pushBack(Sprite::create(ss.str()));
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
        _backgrounds.back()->setPosition(_backgrounds.front()->getContentSize().width * i, 0.f);
        // add to scene
        this->addChild(_backgrounds.back());
    }

    // 뿌연 효과
    auto drawNode = DrawNode::create();
    Vec2 rect[4];
    rect[0] = Vec2(0, 0);
    rect[1] = Vec2(_visibleSize.width, 0);
    rect[2] = Vec2(_visibleSize.width, _visibleSize.height);
    rect[3] = Vec2(0, _visibleSize.height);

    Color4F color(.8f, .95f, .95f, .55f);
    drawNode->drawPolygon(rect, 4, color, 1, color);

    this->addChild(drawNode);

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

    /* 화면 벗어나면 없어지는거.. 바닥 구멍 생기는 장애물때 사용하게 될듯

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
    */

    for(auto tile : _tilePlatforms){
        // move sprites
        tile->setPositionX(tile->getPositionX() - _scrollSpeed * _speedModifier);
        // if the position is completely off the screen, relocate it to the end of the other sprite
        if(tile->getPositionX() <= -tile->getContentSize().width){
            tile->setPositionX(tile->getPositionX() + tile->getContentSize().width * _tilePlatforms.size() / 2); // width * number of sprites / 2 (for top and bottom)
        }
    }

    // obstacles
    for(auto it = _obstacles.begin(); it != _obstacles.end();){
        (*it)->setPositionX((*it)->getPositionX() - _scrollSpeed * _speedModifier);
        if((*it)->getPositionX() < -(*it)->getContentSize().width * 2.f){
            this->removeChild(*it);
            it = _obstacles.erase(it);
        }
        else{
            ++it;
        }
    }

}

void GameLayer::loadPlatforms(){
    // 처음엔 미들 플랫폼으로 위 아래 다 깔아버려
    std::stringstream ss;
    ss << "PNG/Ground/" << _tileSpriteName << "/" << _tileSpriteName << "Half_mid.png";
    auto midTile = Sprite::create(ss.str());
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

    // initial position is midair
    _playerCharacter->setMidAir(true);

    this->addChild(_playerCharacter, 1);
    _defaultPlayerPosX = _visibleSize.width * .33f;

    _playerCharacter->setPosition(_defaultPlayerPosX / 2, _visibleSize.height * .5f);
}

void GameLayer::playerPhysics()
{

    // if player is behind, pull it towards default x position
    if(_playerCharacter->getPositionX() < _defaultPlayerPosX){
        _playerCharacter->setPositionX(MIN(_playerCharacter->getPositionX() + 1.f, _defaultPlayerPosX));
    }

    // maximum falling speed is defined in the header
    _fallSpeed = MIN(_fallSpeed + _fallAcceleration, _maxFallSpeed);

    if(_playerCharacter->isMidAir()){
        _playerCharacter->setPositionY(_playerCharacter->getPositionY() + (_reverseFall * _fallSpeed));

    }
    // check collision
    for(auto tile : _tilePlatforms){
        if(tile->getBoundingBox().intersectsRect(_playerCharacter->getBoundingBox())){
            auto a = _playerCharacter->getBoundingBox();

            _playerCharacter->setMidAir(false);

            // on top of tile
            if(_reverseFall < 0)
                _playerCharacter->setPositionY(tile->getPositionY() + tile->getContentSize().height);
            else
                _playerCharacter->setPositionY(tile->getPositionY() - _playerCharacter->getContentSize().height);

            // reset fall speed
            _fallSpeed = 0.f;
            break;
        }
        else{
            _playerCharacter->setMidAir(true);
        }
    }

    for(auto obs : _obstacles){

		auto obsRect = obs->getBoundingBox();
		auto playerRect = _playerCharacter->getBoundingBox();
        if(obsRect.intersectsRect(playerRect)){
			
			auto left = MAX(obsRect.getMinX(), playerRect.getMinX());
			auto right = MIN(obsRect.getMaxX(), playerRect.getMaxX());
			auto bottom = MAX(obsRect.getMinY(), playerRect.getMinY());
			auto top = MIN(obsRect.getMaxY(), playerRect.getMaxY());

			auto width = right - left;
			auto height = top - bottom;

			auto area = width * height;
			
			// dies only if intersection size if larger than some number
			if (area > 400.f){

				// change sprite to dead
				_playerCharacter->stopAllActions();
				_playerCharacter->setSpriteFrame(Sprite::create("PNG/Enemies/slimePurple_dead.png")->getSpriteFrame());
				_playerCharacter->setFlippedX(true);
				_playerCharacter->setFlippedY(true);
				*_gameState = GAME_STATE::OVER;

				this->pause();
				gameOverSequence();
			}

            break;
        }
    }
}

bool GameLayer::OnTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    switch(*_gameState){
        case GAME_STATE::PLAYING:
            if(!_playerCharacter->isMidAir()){
                // jump
                // reverse fall direction
                _reverseFall *= -1;
                // jump player 
                _fallSpeed = 4.f;
                _playerCharacter->setPositionY(_playerCharacter->getPositionY() + _reverseFall);

                // flip sprite
                _playerCharacter->setFlippedY(!_playerCharacter->isFlippedY());
            }
            break;

        case GAME_STATE::PAUSED:
            // start game
            _playerCharacter->runMoveAnimation();
            *_gameState = GAME_STATE::PLAYING;
            break;

        case GAME_STATE::OVER:

            /************************************************************************/
            /* 사실 게임 오버때는 여기 말고 UI에서 터치 처리해야됨                     */
            /************************************************************************/
            CCLOG("Game over and touched");

            // clean up and reset componets
            this->removeAllChildrenWithCleanup(true);
            _backgrounds.clear();
            _tilePlatforms.clear();
            _obstacles.clear();

            restartComponents();

            *_gameState = GAME_STATE::PAUSED;

            break;

    }


    return false;
}

void GameLayer::scheduleObstacleSpawns(float dt)
{
    // there are 

    auto obs = Sprite::create("PNG/Tiles/boxCrate_single.png");
    obs->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    _obstacles.pushBack(obs);
    this->addChild(obs);
    obs->setPosition(_visibleSize.width, 200.f);

    // randomize spawn rate
    _obstacleSpawnRate = random(2.f, 6.f);

    this->schedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleObstacleSpawns), _obstacleSpawnRate);
}

void GameLayer::gameOverSequence()
{
    auto moveUp = MoveBy::create(.3f, Vec2(0.f, _visibleSize.height * .33f));
    auto moveDown = MoveBy::create(.9f, Vec2(0.f, -_visibleSize.height * 1.5f));

    auto move_ease_out = EaseOut::create(moveUp, 2.f);
    auto move_ease_in = EaseIn::create(moveDown, 4.f);

    auto tempPlayer = _playerCharacter;
    auto deletePlayer = [tempPlayer](){
        tempPlayer->removeFromParentAndCleanup(true);
    };

    auto sequence = Sequence::create(move_ease_out, move_ease_in, deletePlayer, nullptr);

    _playerCharacter->runAction(sequence);

    // enable touch listener
    this->_eventDispatcher->resumeEventListenersForTarget(this);

    // reset score
    *_score = 0.0;
}

void GameLayer::restartComponents()
{
    _scrollSpeed = 3.f;
    _speedModifier = 1.f;

    _fallSpeed = 0.f;

    _reverseFall = -1;

    _obstacleSpawnRate = 8.f;

    //readStageFromFile();
	_obstacleData = GameStageLoader::loadStage(_stageNumber);

    loadBackground();

    loadPlatforms();

    loadCharacter();

    this->resume();
}

void GameLayer::updateScore(float dt)
{
    *_score += _scrollSpeed * _speedModifier * dt;
}
