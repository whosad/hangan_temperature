#include "GameLayer.h"
#include "SimpleAudioEngine.h"
#include "PlayerCharacter.h"
#include "GameStageLoader.h"
#include "GameUILayer.h"

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



    /************************************************************************/
#ifdef _DEBUG	
    auto keyListener = EventListenerKeyboard::create();
    auto pointerToMod = &_speedModifier;
    keyListener->onKeyPressed = [pointerToMod](EventKeyboard::KeyCode keyCode, Event* event){
        if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
            *pointerToMod = 3.f;
        else if(keyCode == EventKeyboard::KeyCode::KEY_M)
            *pointerToMod = 0.f;
    };
    keyListener->onKeyReleased = [pointerToMod](EventKeyboard::KeyCode keyCode, Event* event){
        if(keyCode == EventKeyboard::KeyCode::KEY_SPACE)
            *pointerToMod = 1.f;
        else if(keyCode == EventKeyboard::KeyCode::KEY_M)
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

            // spawning obstacles
            obstacleSpawnFromData(dt);

            // move objects
            scrollGameObjects();

            // player collision 
            playerPhysics();

            // check game over
            isGameOver();

            // score
            updateScore(dt);

            // update gauge
            _playerCharacter->increaseGauge();


            // update passed number of pixels
            _pixelsPassed += _scrollSpeed * _speedModifier;

            // upto 2x times faster than starting speed
            if(_speedModifier > 0.f)
                _speedModifier = MIN(_speedModifier + dt * 0.01f * (1 + _stageNumber), 2.f);

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
    _backgroundLayer->addChild(_backgrounds.front());

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
        _backgroundLayer->addChild(_backgrounds.back());
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

    _backgroundLayer->addChild(drawNode);

}

void GameLayer::scrollGameObjects()
{

    if(_speedModifier <= 0.f)
        return;

    // background sprites
    for(auto bg : _backgrounds){
        // move sprites
        bg->setPositionX(bg->getPositionX() - _scrollSpeed * _speedModifier * .5f);
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
            _obstacleLayer->removeChild(*it, true);
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
        bottom->setPositionY(0.f);

        auto top = Sprite::createWithSpriteFrame(midTile->getSpriteFrame());
        top->setAnchorPoint(Vec2::ZERO);
        top->setPositionX(i * midTile->getContentSize().width);
        top->setPositionY(_visibleSize.height - midTile->getContentSize().height);
        top->setFlippedY(true);

        // add to node
        _platformLayer->addChild(top, 0);
        _platformLayer->addChild(bottom, 0);

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
    _defaultPlayerPosX = _visibleSize.width * .45f;

    _playerCharacter->setPosition(_defaultPlayerPosX * .66f, _visibleSize.height * .5f - 146.f);

    _gameUILayer->setPlayerHealth(_playerCharacter->getHealth());
}

void GameLayer::playerPhysics()
{

    // if player is behind, pull it towards default x position
    if(_playerCharacter->getPositionX() < _defaultPlayerPosX){
        // 1/3 of scrolling speed
        _playerCharacter->setPositionX(MIN(_playerCharacter->getPositionX() + _scrollSpeed * _speedModifier * .33f, _defaultPlayerPosX));
    }

    // maximum falling speed is defined in the header
    _fallSpeed = MIN(_fallSpeed + _fallAcceleration, _maxFallSpeed);

    if(_playerCharacter->isMidAir()){
        _playerCharacter->setPositionY(_playerCharacter->getPositionY() + (_reverseFall * _fallSpeed));

    }

    for(auto tile : _tilePlatforms){

        auto tilePosY = tile->getPositionY();
        auto playerPosY = _playerCharacter->getPositionY();

        // hits ground
        if(tile->getBoundingBox().intersectsRect(_playerCharacter->getBoundingBox())){

            // going top and hits top platform or going bottom and hits bottom
            auto delta = abs(tilePosY - playerPosY);
            // arbitary number 70 (little bigger than player height
            if(delta > 70.f){
                continue;
            }

            _playerCharacter->setMidAir(false);

            // on top of tile
            if(_reverseFall < 0)
                _playerCharacter->setPositionY(tile->getPositionY() + tile->getContentSize().height);
            else
                _playerCharacter->setPositionY(tile->getPositionY());


            // reset fall speed
            _fallSpeed = 0.f;
            break;
        }
        else{
            _playerCharacter->setMidAir(true);
        }
    }

    /*
              obstacle collision

              - check every childern of obstacle node
              - spikes and saws instakill player
              - other boxes push player back

              */
    checkCollision();


    // check if skill is active
    if(_playerCharacter->isEnlarged() && _playerCharacter->getGauge() <= 0.f){
        _playerCharacter->skillEnlarge(1.f, false);
    }

}

void GameLayer::scheduleBeeSpawns(float dt)
{
    // there are 
    auto obstacleNode = Node::create();
    obstacleNode->setName("lethal");

    auto bee = Sprite::create("PNG/Enemies/bee.png");
    auto beeMove = Sprite::create("PNG/Enemies/bee_move.png");

    auto bee0 = bee->getSpriteFrame();
    auto bee1 = beeMove->getSpriteFrame();

    bee->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    beeMove->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    bee0->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    bee1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

    auto moveAnimation = Animation::create();
    moveAnimation->setDelayPerUnit(0.2f);
    moveAnimation->addSpriteFrame(bee0);
    moveAnimation->addSpriteFrame(bee1);

    auto moveAnimate = RepeatForever::create(Animate::create(moveAnimation));

    bee->runAction(moveAnimate);

    obstacleNode->addChild(bee);

    obstacleNode->setPosition(_visibleSize.width + bee->getContentSize().width * 2.f, random(_visibleSize.height * .33f, _visibleSize.height * .66f));
    obstacleNode->setContentSize(Size(bee->getContentSize().width, bee->getContentSize().height));

    // set movement
    // make curves ocassionally
    int numOfCurves = 5;
    Vector<FiniteTimeAction*> jumps;
    // 5 secs
    // 5  / numofjumps
    int sign = random(0, 2) < 1 ? 1 : -1;
    float vY = random(150.f, 300.f);
    for(int i = 0; i < numOfCurves; i++){

        ccBezierConfig bzConfig;
        auto yy = sign * vY;
        bzConfig.controlPoint_1 = Point(-100.f, yy);
        bzConfig.controlPoint_2 = Point(-200.f, yy);
        bzConfig.endPosition = Point(-300.f, 0);

        auto bezierBy = BezierBy::create(1.75f, bzConfig);
        sign *= -1;
        jumps.pushBack(bezierBy);
    }

    // spawn both actions
    obstacleNode->runAction(Sequence::create(jumps));
    obstacleNode->setContentSize(bee->getContentSize());

#ifdef COCOS2D_DEBUG
    auto box = DrawNode::create();
    auto bb = obstacleNode->getBoundingBox();
    box->drawRect(Vec2::ZERO, bb.size, Color4F::RED);
    box->setLineWidth(2);
    obstacleNode->addChild(box);
#endif
    obstacleNode->setScale(.35f);

    _obstacles.pushBack(obstacleNode);
    _obstacleLayer->addChild(obstacleNode, 3);


    // randomize spawn rate
    _beeSpawnRate = random(8.f, 13.f);

    this->schedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleBeeSpawns), _beeSpawnRate);
}

void GameLayer::scheduleRandomGust(float dt)
{
    // clouds spawn and gust pushes

    int numOfClouds = random(4, 7);
    for(int i = 0; i < numOfClouds; i++){
        auto cloud = Sprite::create("PNG/cloud.png");

        cloud->setPosition(_visibleSize.width + random(50.f, 500.f), random(0.f, _visibleSize.height * .9f));
        ccBezierConfig bzConfig;
        bzConfig.controlPoint_1 = Vec2(-400.f - random(50.f, 500.f), -random(50.f, 100.f));
        bzConfig.controlPoint_2 = Vec2(-800.f - random(50.f, 500.f), -random(0.f, 50.f));
        bzConfig.endPosition = Vec2(-1200.f - random(50.f, 500.f), _visibleSize.height);
        auto bzBy = BezierBy::create(2.f, bzConfig);
        cloud->runAction(Sequence::create(bzBy, RemoveSelf::create(true), nullptr));

        cloud->setOpacity(170);

        this->addChild(cloud);
    }


    // random gust pushes player back 
    if(!_playerCharacter->isEnlarged()){
        auto moveBy = MoveBy::create(2.f, Vec2(-random(400.f, 700.f), 0.f));
        _playerCharacter->runAction(moveBy);
        moveBy->setTag(31);
    }

    // randomize spawn rate
    _gustSpawnRate = random(15.f, 20.f);

    this->schedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleRandomGust), _gustSpawnRate);
}




void GameLayer::gameOverSequence()
{
    // dying animation
    auto moveUp = MoveBy::create(.3f, Vec2(0.f, _visibleSize.height * .15f));
    auto moveDown = MoveBy::create(.9f, Vec2(0.f, -_visibleSize.height * 1.15f));

    auto move_ease_out = EaseOut::create(moveUp, 2.f);
    auto move_ease_in = EaseIn::create(moveDown, 4.f);

    auto tempPlayer = _playerCharacter;
    auto deletePlayer = [tempPlayer](){
        tempPlayer->removeFromParentAndCleanup(true);
    };

    auto sequence = Sequence::create(move_ease_out, move_ease_in, deletePlayer, nullptr);

    _playerCharacter->runAction(sequence);

    stopGameState();
}

void GameLayer::restartComponents()
{
    _scrollSpeed = 5.f;
    _speedModifier = 1.f;

    _fallSpeed = 0.f;

    _reverseFall = -1;

    _beeSpawnRate = 8.f;
    _gustSpawnRate = 15.f;

    _pixelsPassed = 0.f;

    //readStageFromFile();
    _obstacleData = GameStageLoader::loadStage(_stageNumber);

    *_score = 0.0;

    _backgroundLayer = Node::create();
    _obstacleLayer = Node::create();
    _platformLayer = Node::create();

    _wonGame = false;

    this->addChild(_backgroundLayer);
    this->addChild(_obstacleLayer);
    this->addChild(_platformLayer);

    _gameUILayer->setInstruction(true);

    loadBackground();

    loadPlatforms();

    loadCharacter();

    this->resume();


}

void GameLayer::updateScore(float dt)
{
    if(!_wonGame){
        // falling behind decreases score gained
        *_score += (_playerCharacter->getPositionX() / 10) * dt;
    }
}

void GameLayer::startGame()
{
    // start animation
    _playerCharacter->runMoveAnimation();

    // change game state
    *_gameState = GAME_STATE::PLAYING;



}

void GameLayer::obstacleSpawnFromData(float dt)
{
    // data is not empty
    if(_obstacleData.empty())
        return;

    // subtract traveled distance
    _obstacleData.front().first -= (_speedModifier * _scrollSpeed);

    // while distance is <= 0
    while(_obstacleData.front().first <= 0.f){
        // spawn data.second
        spawnObstacle();

        // remove
        _obstacleData.erase(_obstacleData.begin());

        if(_obstacleData.empty())
            return;

        _obstacleData.front().first -= (_speedModifier * _scrollSpeed);

    }
}

void GameLayer::spawnObstacle()
{
    OBSTACLE_TYPE obstacleType = _obstacleData.front().second;

    auto posX = _visibleSize.width + 50.f - _obstacleData.front().first;

    CCLOG("delat: %f", _obstacleData.front().first);

    auto obstacleNode = Node::create();
    obstacleNode->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

    switch(obstacleType)
    {
        // singe box top and bottom
        case TAG_SINGLE_BOX_T:
        {
            auto box1 = Sprite::create("PNG/Tiles/boxCrate_double.png");

            box1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            box1->setPositionY(0.f);

            obstacleNode->addChild(box1);

            obstacleNode->setPosition(posX, _visibleSize.height - _tilePlatforms.front()->getContentSize().height - box1->getContentSize().height);
            obstacleNode->setContentSize(Size(box1->getContentSize().width, box1->getContentSize().height));

        }
        break;

        case TAG_SINGLE_BOX_B:
        {
            // 3 boxes
            auto box1 = Sprite::create("PNG/Tiles/boxCrate_double.png");

            box1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            box1->setPositionY(0.f);

            obstacleNode->addChild(box1);

            obstacleNode->setPosition(posX, _tilePlatforms.front()->getContentSize().height);
            obstacleNode->setContentSize(Size(box1->getContentSize().width, box1->getContentSize().height));
        }
        break;

        // boxes top and bottom
        case TAG_BOX_T:
        {
            // 3 boxes
            auto box1 = Sprite::create("PNG/Tiles/boxCrate_double.png");
            auto box2 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());
            auto box3 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());

            box1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            box2->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            box3->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            box1->setPositionY(0.f);
            box2->setPositionY(box1->getContentSize().height);
            box3->setPositionY(box1->getContentSize().height * 2.f);

            obstacleNode->addChild(box1);
            obstacleNode->addChild(box2);
            obstacleNode->addChild(box3);

            obstacleNode->setPosition(posX, _visibleSize.height - _tilePlatforms.front()->getContentSize().height - box1->getContentSize().height * 3.f);
            obstacleNode->setContentSize(Size(box1->getContentSize().width, box1->getContentSize().height * 3.f));



        }
        break;
        case TAG_BOX_B:
        {
            // 3 boxes
            auto box1 = Sprite::create("PNG/Tiles/boxCrate_double.png");
            auto box2 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());
            auto box3 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());

            box1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            box2->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            box3->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            box1->setPositionY(0.f);
            box2->setPositionY(box1->getContentSize().height);
            box3->setPositionY(box1->getContentSize().height * 2.f);

            obstacleNode->addChild(box1);
            obstacleNode->addChild(box2);
            obstacleNode->addChild(box3);

            obstacleNode->setPosition(posX, _tilePlatforms.front()->getContentSize().height);
            obstacleNode->setContentSize(Size(box1->getContentSize().width, box1->getContentSize().height * 3.f));

        }
        break;
        case TAG_STAIR_T:
        {
            // 3 2 1 boxes
            // 3 boxes
            auto box1 = Sprite::create("PNG/Tiles/boxCrate_double.png");
            auto box2 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());
            auto box3 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());

            box1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            box2->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            box3->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            box1->setPositionY(0.f);
            box2->setPositionY(box1->getContentSize().height);
            box3->setPositionY(box1->getContentSize().height * 2.f);

            obstacleNode->addChild(box1);
            obstacleNode->addChild(box2);
            obstacleNode->addChild(box3);

            obstacleNode->setPosition(posX, _visibleSize.height - _tilePlatforms.front()->getContentSize().height - box1->getContentSize().height * 3.f);
            obstacleNode->setContentSize(Size(box1->getContentSize().width, box1->getContentSize().height * 3.f));


            // add 3 obstacle nodes
            auto obstacleNode1 = Node::create();

            // 2 boxes
            auto box4 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());
            auto box5 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());

            box4->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            box5->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            box4->setPositionY(0.f);
            box5->setPositionY(box1->getContentSize().height);

            obstacleNode1->addChild(box4);
            obstacleNode1->addChild(box5);

            obstacleNode1->setPosition(posX + box1->getContentSize().width, _visibleSize.height - _tilePlatforms.front()->getContentSize().height - box1->getContentSize().height * 2.f);
            obstacleNode1->setContentSize(Size(box1->getContentSize().width, box1->getContentSize().height * 2.f));

            _obstacles.pushBack(obstacleNode1);
            _obstacleLayer->addChild(obstacleNode1);

            // 1 box
            auto obstacleNode2 = Node::create();

            auto box6 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());

            box6->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            box6->setPositionY(0.f);

            obstacleNode2->addChild(box6);

            obstacleNode2->setPosition(posX + box1->getContentSize().width*2.f, _visibleSize.height - _tilePlatforms.front()->getContentSize().height - box1->getContentSize().height);
            obstacleNode2->setContentSize(Size(box1->getContentSize().width, box1->getContentSize().height));

            _obstacles.pushBack(obstacleNode2);
            _obstacleLayer->addChild(obstacleNode2);
        }
        break;
        case TAG_STAIR_B:
        {
            // 3 2 1 boxes
            // 3 boxes
            auto box1 = Sprite::create("PNG/Tiles/boxCrate_double.png");
            auto box2 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());
            auto box3 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());

            box1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            box2->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            box3->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            box1->setPositionY(0.f);
            box2->setPositionY(box1->getContentSize().height);
            box3->setPositionY(box1->getContentSize().height * 2.f);

            obstacleNode->addChild(box1);
            obstacleNode->addChild(box2);
            obstacleNode->addChild(box3);

            obstacleNode->setPosition(posX + box1->getContentSize().width*2.f, _tilePlatforms.front()->getContentSize().height);
            obstacleNode->setContentSize(Size(box1->getContentSize().width, box1->getContentSize().height * 3.f));


            // add 3 obstacle nodes
            auto obstacleNode1 = Node::create();

            // 2 boxes
            auto box4 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());
            auto box5 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());

            box4->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            box5->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            box4->setPositionY(0.f);
            box5->setPositionY(box1->getContentSize().height);

            obstacleNode1->addChild(box4);
            obstacleNode1->addChild(box5);

            obstacleNode1->setPosition(posX + box1->getContentSize().width, _tilePlatforms.front()->getContentSize().height);
            obstacleNode1->setContentSize(Size(box1->getContentSize().width, box1->getContentSize().height * 2.f));

            _obstacles.pushBack(obstacleNode1);
            _obstacleLayer->addChild(obstacleNode1);

            // 1 box
            auto obstacleNode2 = Node::create();

            auto box6 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());

            box6->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            box6->setPositionY(0.f);

            obstacleNode2->addChild(box6);

            obstacleNode2->setPosition(posX, _tilePlatforms.front()->getContentSize().height);
            obstacleNode2->setContentSize(Size(box1->getContentSize().width, box1->getContentSize().height));

            _obstacles.pushBack(obstacleNode2);
            _obstacleLayer->addChild(obstacleNode2);
        }
        break;
        case TAG_SAW_T:
        {
            obstacleNode->setName("lethal");

            auto saw = Sprite::create("PNG/Enemies/sawHalf.png");
            auto sawMove = Sprite::create("PNG/Enemies/sawHalf_move.png");

            saw->setFlippedY(true);
            sawMove->setFlippedY(true);

            auto saw0 = saw->getSpriteFrame();
            auto saw1 = sawMove->getSpriteFrame();

            saw0->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            saw1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);


            auto moveAnimation = Animation::create();
            moveAnimation->setDelayPerUnit(0.05f);
            moveAnimation->addSpriteFrame(saw0);
            moveAnimation->addSpriteFrame(saw1);


            auto moveAnimate = RepeatForever::create(Animate::create(moveAnimation));

            saw->runAction(moveAnimate);

            saw->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            saw->setPositionY(0.f);

            obstacleNode->addChild(saw);

            obstacleNode->setPosition(posX, _visibleSize.height - _tilePlatforms.front()->getContentSize().height - saw->getContentSize().height);
            obstacleNode->setContentSize(saw->getContentSize());


            // set movement
            auto moveBy = MoveBy::create(1.f, Vec2(55.f, 0.f));
            auto moveByRev = moveBy->clone()->reverse();

            auto repeatForever = RepeatForever::create(Sequence::create(moveBy, moveByRev, nullptr));
            obstacleNode->runAction(repeatForever);

        }
        break;
        case TAG_SAW_B:{
            obstacleNode->setName("lethal");

            auto saw = Sprite::create("PNG/Enemies/sawHalf.png");
            auto sawMove = Sprite::create("PNG/Enemies/sawHalf_move.png");

            auto saw0 = saw->getSpriteFrame();
            auto saw1 = sawMove->getSpriteFrame();

            saw0->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            saw1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);


            auto moveAnimation = Animation::create();
            moveAnimation->setDelayPerUnit(0.05f);
            moveAnimation->addSpriteFrame(saw0);
            moveAnimation->addSpriteFrame(saw1);


            auto moveAnimate = RepeatForever::create(Animate::create(moveAnimation));

            saw->runAction(moveAnimate);

            saw->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            saw->setPositionY(0.f);

            obstacleNode->addChild(saw);

            obstacleNode->setPosition(posX, _tilePlatforms.front()->getContentSize().height);
            obstacleNode->setContentSize(saw->getContentSize());

            // set movement
            auto moveBy = MoveBy::create(1.f, Vec2(55.f, 0.f));
            auto moveByRev = moveBy->clone()->reverse();

            auto repeatForever = RepeatForever::create(Sequence::create(moveBy, moveByRev, nullptr));
            obstacleNode->runAction(repeatForever);
        }
                       break;
        case TAG_SPIKE_T:
        {
            obstacleNode->setName("lethal");

            auto spike = Sprite::create("PNG/Tiles/spikes.png");
            spike->setFlippedY(true);

            spike->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            spike->setPositionY(0.f);

            obstacleNode->addChild(spike);

            obstacleNode->setPosition(posX, _visibleSize.height - _tilePlatforms.front()->getContentSize().height - spike->getContentSize().height * .95f);
            obstacleNode->setContentSize(spike->getContentSize());

            // set movement
            auto moveBy = MoveBy::create(1.f, Vec2(.0f, spike->getContentSize().height));
            auto moveByRev = MoveBy::create(0.5f, Vec2(.0f, -spike->getContentSize().height));

            auto repeatForever = RepeatForever::create(Sequence::create(moveBy, DelayTime::create(1.25f), moveByRev, nullptr));
            obstacleNode->runAction(repeatForever);

        }			break;
        case TAG_SPIKE_B:
        {
            obstacleNode->setName("lethal");

            auto spike = Sprite::create("PNG/Tiles/spikes.png");

            spike->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

            spike->setPositionY(0.f);

            obstacleNode->addChild(spike);

            obstacleNode->setPosition(posX, _tilePlatforms.front()->getContentSize().height  * .95f);
            obstacleNode->setContentSize(spike->getContentSize());

            // set movement
            auto moveBy = MoveBy::create(1.f, Vec2(.0f, -spike->getContentSize().height));
            auto moveByRev = MoveBy::create(0.5f, Vec2(.0f, spike->getContentSize().height));

            auto repeatForever = RepeatForever::create(Sequence::create(moveBy, DelayTime::create(1.25f), moveByRev, nullptr));
            obstacleNode->runAction(repeatForever);
        }
        break;
        case TAG_WEIGHT:
            break;
    }

#ifdef COCOS2D_DEBUG
    auto box = DrawNode::create();
    auto bb = obstacleNode->getBoundingBox();
    box->drawRect(Vec2::ZERO, bb.size, Color4F::RED);
    box->setLineWidth(2);
    obstacleNode->addChild(box);
#endif

    _obstacles.pushBack(obstacleNode);
    _obstacleLayer->addChild(obstacleNode);

}

void GameLayer::checkCollision()
{
    //for (auto obs : _obstacles){
    for(auto it = _obstacles.begin(); it != _obstacles.end();){
        // if player is enlarged, breakes (*it)tacles
        if(_playerCharacter->isEnlarged()){


            auto obsRect = (*it)->getBoundingBox();
            auto playerBoundingBox = _playerCharacter->getBoundingBox();

            // 10% off from left
            playerBoundingBox.origin += Vec2(playerBoundingBox.size.width * .1f, 0.f);
            // 80% of the original bounding box
            playerBoundingBox.size = Vec2(playerBoundingBox.size.width * .8f, playerBoundingBox.size.height);

            // if collides
            if(playerBoundingBox.intersectsRect(obsRect)){

                auto brown = Sprite::create("PNG/Particles/brickBrown.png");
                auto grey = Sprite::create("PNG/Particles/brickGrey.png");

                auto bb = (*it)->getBoundingBox();
                bb.origin = (*it)->getPosition();
                int area = (*it)->getBoundingBox().size.width * (*it)->getBoundingBox().size.height;
                auto min = MIN(5, area / 1000);
                auto max = MAX(5, area / 1000);
                auto num = random(min, max);

                for(int i = 0; i < num; i++){
                    // random pos
                    auto posX = random(bb.getMinX(), bb.getMaxX());
                    auto posY = random(bb.getMinY(), bb.getMaxY());

                    Sprite* brick;
                    // brown
                    if((int)posX % 2 == 0){
                        brick = Sprite::createWithSpriteFrame(brown->getSpriteFrame());
                    }
                    // grey
                    else{
                        brick = Sprite::createWithSpriteFrame(grey->getSpriteFrame());
                    }

                    MoveBy* moveBy;
                    // left or right
                    if(posX > bb.getMidX()){
                        moveBy = MoveBy::create(.75f, Vec2(random(50.f, 400.f), 0.f));
                    }
                    else{
                        moveBy = MoveBy::create(.75f, Vec2(-random(50.f, 400.f), 0.f));
                    }
                    auto moveUp = EaseOut::create(MoveBy::create(.25f, Vec2(0.f, random(200.f, 400.f))), 3.f);
                    auto moveDown = EaseIn::create(MoveBy::create(.75f, Vec2(0.f, -_visibleSize.height)), 2.f);

                    auto spawn = Spawn::create(moveBy, moveUp, moveDown, nullptr);
                    auto seq = Sequence::create(spawn, RemoveSelf::create(true), nullptr);
                    brick->runAction(seq);
                    brick->setPosition(posX, posY);
                    brick->setGlobalZOrder(2);
                    this->addChild(brick);

                }
                // remove from vector
                (*it)->removeFromParentAndCleanup(true);
                it = _obstacles.erase(it);
            }
            // has not collided
            else{
                ++it;
            }
        }
        else{

            // check if the object is lethal
            if((*it)->getName() == "lethal"){

                // lethal objects have slightly smaller bounding box for easier difficulty 
                auto obsRect = (*it)->getBoundingBox();
                // certain % off from all four sides
                float offPercent = 0.2f;
                obsRect.origin += obsRect.size * offPercent;
                obsRect.size = obsRect.size * (1.f - offPercent * 2.f);

                auto playerRect = _playerCharacter->getBoundingBox();


                auto playerBoundingBox = _playerCharacter->getBoundingBox();


                // 10% off from left
                playerBoundingBox.origin += Vec2(playerBoundingBox.size.width * .1f, 0.f);
                // 80% of the original bounding box
                playerBoundingBox.size = Vec2(playerBoundingBox.size.width * .8f, playerBoundingBox.size.height);

                if(obsRect.intersectsRect(playerBoundingBox)){
                    // only triggers when player is hurtable
                    if(!_playerCharacter->isHit()){
                        _playerCharacter->setHit(true);
                        _gameUILayer->updateHealth();

                        if(_playerCharacter->isDead()){
                            // change sprite to dead
                            _playerCharacter->stopAllActions();
                            _playerCharacter->setSpriteFrame(Sprite::create("PNG/Enemies/slimePurple_dead.png")->getSpriteFrame());
                            _playerCharacter->setFlippedX(true);
                            _playerCharacter->setFlippedY(true);

                            this->pause();
                            gameOverSequence();
                        }
                        else{
                            _playerCharacter->runBlink();
                            // loses score
                            *_score = MAX(0.0, *_score - 50.0);
                        }

                        // return;
                    }
                }
            }

            // non-lethal (*it)tacles
            else{
                auto obsRect = (*it)->getBoundingBox();
                auto playerBoundingBox = _playerCharacter->getBoundingBox();


                // 10% off from left
                playerBoundingBox.origin += Vec2(playerBoundingBox.size.width * .1f, 0.f);
                // 80% of the original bounding box
                playerBoundingBox.size = Vec2(playerBoundingBox.size.width * .8f, playerBoundingBox.size.height);

                // if collides
                if(obsRect.intersectsRect(playerBoundingBox)){

                    // it pushes player when collided on side (left only)

                    // side way, slide
                    if(obsRect.getMinX() >= _playerCharacter->getPositionX() + _playerCharacter->getContentSize().width * .3f &&
                       (_reverseFall > 0 ? _playerCharacter->getPositionY() > obsRect.getMinY() : _playerCharacter->getPositionY() < obsRect.getMaxY())){

                        _playerCharacter->setPositionX(obsRect.getMinX() - _playerCharacter->getContentSize().width * .9f * .5f);
                    }
                    else{
                        // player can stand on non lethal (*it)tacles.

                        // colliding from top
                        _fallSpeed = 0.f;
                        _playerCharacter->setMidAir(false);

                        _playerCharacter->setPositionY((_reverseFall > 0) ? obsRect.getMinY() : obsRect.getMaxY());


                    }

                }
            }

            // next
            ++it;
        }
    }
}

void GameLayer::isGameOver()
{
    // player dies if goes past game sceern

    if(_playerCharacter->getPositionX() <= 0.f){
        //die
        // change sprite to dead
        _playerCharacter->stopAllActions();
        _playerCharacter->setSpriteFrame(Sprite::create("PNG/Enemies/slimePurple_dead.png")->getSpriteFrame());
        _playerCharacter->setFlippedX(true);
        _playerCharacter->setFlippedY(true);

        this->pause();
        gameOverSequence();

    }

    // all obstacles are gone => wins
    if(_obstacles.size() <= 0 && _obstacleData.size() <= 0){
        CCLOG("wins");
        if(!_wonGame)
            winGameSequence();
    }
}


void GameLayer::winGameSequence(){
    _wonGame = true;
    // unlock next stage
    UserDefault::getInstance()->setIntegerForKey("unlockedStage", _stageNumber + 1);

    // castle and dude
    showCastleAndDude();

    this->unschedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleBeeSpawns));
    this->unschedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleRandomGust));
    // decrease speed mod
    this->schedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleLerpSpeedmod), .09f);

    // actions to celebrate 


    auto star = Sprite::create("PNG/Items/star.png");
    auto spin = ScaleTo::create(.15f, .0f, 1.f, 1.f);
    auto spinBack = ScaleTo::create(.15f, 1.f, 1.f, 1.f);
    auto rpt = RepeatForever::create(Sequence::create(spin, spinBack, nullptr));
    for(int i = 0; i < 40; i++){
        auto starClone = Sprite::createWithSpriteFrame(star->getSpriteFrame());
        starClone->setPosition(_visibleSize.width / 2 + random(-400, 400), _visibleSize.height / 2 + random(-250, 250));
        starClone->setGlobalZOrder(3);
        starClone->setScaleX(random(.0f, 1.f));

        MoveBy* moveBy;
        // left or right
        if(starClone->getPositionX() >= _visibleSize.width / 2){
            moveBy = MoveBy::create(1.75f, Vec2(random(0.f, 400.f), 0.f));
        }
        else{
            moveBy = MoveBy::create(1.75f, Vec2(-random(0.f, 400.f), 0.f));
        }
        auto moveUp = EaseOut::create(MoveBy::create(1.25f, Vec2(0.f, random(200.f, 400.f))), 3.f);
        auto moveDown = EaseIn::create(MoveBy::create(1.75f, Vec2(0.f, -_visibleSize.height)), 2.f);

        auto spawn = Spawn::create(moveBy, moveUp, moveDown, nullptr);
        auto seq = Sequence::create(spawn, RemoveSelf::create(true), nullptr);


        starClone->runAction(seq);
        starClone->runAction(rpt->clone());
        this->addChild(starClone);
    }


    auto seq = Sequence::create(DelayTime::create(10.f), CallFunc::create(CC_CALLBACK_0(GameLayer::stopGameState, this)), nullptr);

    this->runAction(seq);


}

void GameLayer::stopGameState()
{

    // pause the game
    this->pause();

    // sets state to game over
    *_gameState = GAME_STATE::OVER;

    this->unschedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleBeeSpawns));
    this->unschedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleRandomGust));

    // enable touch listener
    this->_eventDispatcher->resumeEventListenersForTarget(this);

}

void GameLayer::scheduleLerpSpeedmod(float dt)
{

    if(_speedModifier <= .1f){
        _speedModifier = 0.f;
        this->unschedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleLerpSpeedmod));
    }

    _speedModifier *= .9f;
}

void GameLayer::showCastleAndDude(){

    // right most tile
    Sprite* rmt = _tilePlatforms.front();
    for(auto tile : _tilePlatforms){
        if(tile->getPositionY() > _visibleSize.height / 2) continue;
        if(rmt->getPositionX() < tile->getPositionX())
            rmt = tile;
    }

    // castle
    auto castleL = Sprite::create("PNG/Ending/castleLeftHalf.png");
    castleL->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    castleL->setPosition(rmt->getContentSize());
    rmt->addChild(castleL);

    // add dude
    auto dude = Sprite::create("PNG/Ending/soldier_stand.png");
    dude->setFlippedX(true);
    dude->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    dude->setPositionY(castleL->getPositionY());
    dude->setPositionX(castleL->getPositionX() + dude->getContentSize().width);
    rmt->addChild(dude);

    auto animation = Animation::create();
    animation->setDelayPerUnit(.5f);
    animation->addSpriteFrameWithFile("PNG/Ending/soldier_cheer1.png");
    animation->addSpriteFrameWithFile("PNG/Ending/soldier_cheer2.png");
    animation->setLoops(1);
    
    auto animate = Animate::create(animation);

    auto jumpBy = JumpBy::create(1.f, Vec2::ZERO, 100.f, 1);

    auto spawn = Spawn::create(jumpBy, animate, nullptr);

    dude->runAction(Sequence::create(DelayTime::create(3.f), spawn, nullptr));

    auto castleR = Sprite::create("PNG/Ending/castleRightHalf.png");
    castleR->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    castleR->setPosition(rmt->getContentSize() + Size(castleL->getContentSize().width, 0.f));
    rmt->addChild(castleR);
}