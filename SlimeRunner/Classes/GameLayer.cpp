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
	if (!Node::init())
	{
		return false;
	}

	CCLOG("Game Layer loaded");

	// set visibe size
	_visibleSize = Director::getInstance()->getVisibleSize();


	// set obstacle spawn schedule
	//this->schedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleObstacleSpawns), 1);

	// add touch listeners
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameLayer::OnTouchBegan, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);


	/************************************************************************/
#ifdef _DEBUG	
	auto keyListener = EventListenerKeyboard::create();
	auto pointerToMod = &_scrollSpeed;
	keyListener->onKeyPressed = [pointerToMod](EventKeyboard::KeyCode keyCode, Event* event){
		if (keyCode == EventKeyboard::KeyCode::KEY_SPACE)
			*pointerToMod = 10.f;
	};
	keyListener->onKeyReleased = [pointerToMod](EventKeyboard::KeyCode keyCode, Event* event){
		if (keyCode == EventKeyboard::KeyCode::KEY_SPACE)
			*pointerToMod = 5.f;
	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);


#endif
	/************************************************************************/

 

	return true;
}

void GameLayer::update(float dt){

	switch (*_gameState)
	{
		case GAME_STATE::PLAYING:

			// spawning obstacles
			obstacleSpawnFromData(dt);

			// move objects
			scrollGameObjects();

			// player collision 
			playerPhysics();

			// score
			updateScore(dt);

			// update passed number of pixels
			_pixelsPassed += _scrollSpeed * _speedModifier;

			_speedModifier += dt * 0.01f;

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
	for (int i = 1; i < bgSize; i++){
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
	// background sprites
	for (auto bg : _backgrounds){
		// move sprites
		bg->setPositionX(bg->getPositionX() - _scrollSpeed * _speedModifier * .5f);
		// if the position is completely off the screen, relocate it to the end of the other sprite
		if (bg->getPositionX() <= -bg->getContentSize().width){
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

	for (auto tile : _tilePlatforms){
		// move sprites
		tile->setPositionX(tile->getPositionX() - _scrollSpeed * _speedModifier);
		// if the position is completely off the screen, relocate it to the end of the other sprite
		if (tile->getPositionX() <= -tile->getContentSize().width){
			tile->setPositionX(tile->getPositionX() + tile->getContentSize().width * _tilePlatforms.size() / 2); // width * number of sprites / 2 (for top and bottom)
		}
	}

	// obstacles
	for (auto it = _obstacles.begin(); it != _obstacles.end();){
		(*it)->setPositionX((*it)->getPositionX() - _scrollSpeed * _speedModifier);
		if ((*it)->getPositionX() < -(*it)->getContentSize().width * 2.f){
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

	for (int i = 0; i < numOfPlatforms; i++){

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

	_playerCharacter->setPosition(_defaultPlayerPosX / 2, _visibleSize.height * .5f);

}

void GameLayer::playerPhysics()
{

	// if player is behind, pull it towards default x position
	if (_playerCharacter->getPositionX() < _defaultPlayerPosX){
		_playerCharacter->setPositionX(MIN(_playerCharacter->getPositionX() + 1.f, _defaultPlayerPosX));
	}

	// maximum falling speed is defined in the header
	_fallSpeed = MIN(_fallSpeed + _fallAcceleration, _maxFallSpeed);

	if (_playerCharacter->isMidAir()){
		_playerCharacter->setPositionY(_playerCharacter->getPositionY() + (_reverseFall * _fallSpeed));

	}

	for (auto tile : _tilePlatforms){
		if (tile->getBoundingBox().intersectsRect(_playerCharacter->getBoundingBox())){
			auto a = _playerCharacter->getBoundingBox();

			_playerCharacter->setMidAir(false);

			// on top of tile
			if (_reverseFall < 0)
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

	/*
		obstacle collision

		- check every childern of obstacle node
		- spikes and saws instakill player
		- other boxes push player back

		*/
	checkCollision();



	// player dies if goes past game sceern
	if (_playerCharacter->getPositionX() + _playerCharacter->getContentSize().width / 2.f <= 0.f){
		//die
		// change sprite to dead
		_playerCharacter->stopAllActions();
		_playerCharacter->setSpriteFrame(Sprite::create("PNG/Enemies/slimePurple_dead.png")->getSpriteFrame());
		_playerCharacter->setFlippedX(true);
		_playerCharacter->setFlippedY(true);
		*_gameState = GAME_STATE::OVER;

		this->pause();
		gameOverSequence();
	}

	// store last position of the player
	_lastPosition = _playerCharacter->getPosition();
}

bool GameLayer::OnTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	switch (*_gameState){
		case GAME_STATE::PLAYING:
			if (!_playerCharacter->isMidAir()){
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
			startGame();


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
	_obstacleLayer->addChild(obs);
	obs->setPosition(_visibleSize.width, 200.f);

	// randomize spawn rate
	_obstacleSpawnRate = random(2.f, 6.f);

	this->schedule(CC_SCHEDULE_SELECTOR(GameLayer::scheduleObstacleSpawns), _obstacleSpawnRate);
}

void GameLayer::gameOverSequence()
{
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

	// enable touch listener
	this->_eventDispatcher->resumeEventListenersForTarget(this);

}

void GameLayer::restartComponents()
{
	_scrollSpeed = 5.f;
	_speedModifier = 1.f;

	_fallSpeed = 0.f;

	_reverseFall = -1;

	_obstacleSpawnRate = 8.f;

	_pixelsPassed = 0.f;

	//readStageFromFile();
	_obstacleData = GameStageLoader::loadStage(_stageNumber);

       *_score = 0.0;

       _backgroundLayer = Node::create();
       _obstacleLayer = Node::create();
       _platformLayer = Node::create();

       this->addChild(_backgroundLayer);
       this->addChild(_obstacleLayer);
       this->addChild(_platformLayer);

	loadBackground();

	loadPlatforms();

	loadCharacter();



	this->resume();
}

void GameLayer::updateScore(float dt)
{
	*_score += _scrollSpeed * _speedModifier * dt * 5.f;
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
	// keep spawning while there is data
	if (!_obstacleData.empty()){

		// if pixel count has reached
		if (_obstacleData.front().first <= 0.f){

			// spawn data.second 
			spawnObstacle(_obstacleData.front().second);

			// remove
			_obstacleData.erase(_obstacleData.begin());

		}
		else{
			_obstacleData.front().first -= _speedModifier * _scrollSpeed;
		}

	}
}

void GameLayer::spawnObstacle(OBSTACLE_TYPE obstacleType)
{
	auto posX = _visibleSize.width + 50.f + _obstacleData.front().first;

	auto obstacleNode = Node::create();
	obstacleNode->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	switch (obstacleType)
	{
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
			_obstacleLayer->addChild(obstacleNode1, 0);

			// 1 box
			auto obstacleNode2 = Node::create();

			auto box6 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());

			box6->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

			box6->setPositionY(0.f);

			obstacleNode2->addChild(box6);

			obstacleNode2->setPosition(posX + box1->getContentSize().width*2.f, _visibleSize.height - _tilePlatforms.front()->getContentSize().height - box1->getContentSize().height);
			obstacleNode2->setContentSize(Size(box1->getContentSize().width, box1->getContentSize().height));

			_obstacles.pushBack(obstacleNode2);
			_obstacleLayer->addChild(obstacleNode2, 0);
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
			_obstacleLayer->addChild(obstacleNode1, 0);

			// 1 box
			auto obstacleNode2 = Node::create();

			auto box6 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());

			box6->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

			box6->setPositionY(0.f);

			obstacleNode2->addChild(box6);

			obstacleNode2->setPosition(posX, _tilePlatforms.front()->getContentSize().height);
			obstacleNode2->setContentSize(Size(box1->getContentSize().width, box1->getContentSize().height));

			_obstacles.pushBack(obstacleNode2);
			_obstacleLayer->addChild(obstacleNode2, 0);
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
			moveAnimation->setDelayPerUnit(0.15f);
			moveAnimation->addSpriteFrame(saw0);
			moveAnimation->addSpriteFrame(saw1);


			auto moveAnimate = RepeatForever::create(Animate::create(moveAnimation));

			saw->runAction(moveAnimate);

			saw->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

			saw->setPositionY(0.f);

			obstacleNode->addChild(saw);

			obstacleNode->setPosition(posX, _visibleSize.height - _tilePlatforms.front()->getContentSize().height - saw->getContentSize().height);
			obstacleNode->setContentSize(Size(saw->getContentSize().width, saw->getContentSize().height));


			// set movement
			auto moveBy = MoveBy::create(1.f, Vec2(50.f, 0.f));
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
			moveAnimation->setDelayPerUnit(0.15f);
			moveAnimation->addSpriteFrame(saw0);
			moveAnimation->addSpriteFrame(saw1);


			auto moveAnimate = RepeatForever::create(Animate::create(moveAnimation));

			saw->runAction(moveAnimate);

			saw->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

			saw->setPositionY(0.f);

			obstacleNode->addChild(saw);

			obstacleNode->setPosition(posX, _tilePlatforms.front()->getContentSize().height);
			obstacleNode->setContentSize(Size(saw->getContentSize().width, saw->getContentSize().height));

			// set movement
			auto moveBy = MoveBy::create(1.f, Vec2(50.f, 0.f));
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

			obstacleNode->setPosition(posX, _visibleSize.height - _tilePlatforms.front()->getContentSize().height - spike->getContentSize().height);
			obstacleNode->setContentSize(Size(spike->getContentSize().width, spike->getContentSize().height));

			// set movement
			auto moveBy = MoveBy::create(1.f, Vec2(.0f, spike->getContentSize().height * .8f));
			auto moveByRev = MoveBy::create(0.5f, Vec2(.0f, -spike->getContentSize().height * .8f));

			auto repeatForever = RepeatForever::create(Sequence::create(moveBy, moveByRev, DelayTime::create(1.f), nullptr));
			obstacleNode->runAction(repeatForever);

		}			break;
		case TAG_SPIKE_B:
		{
			obstacleNode->setName("lethal");

			auto spike = Sprite::create("PNG/Tiles/spikes.png");
		
			spike->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

			spike->setPositionY(0.f);

			obstacleNode->addChild(spike);

			obstacleNode->setPosition(posX, _tilePlatforms.front()->getContentSize().height);
			obstacleNode->setContentSize(Size(spike->getContentSize().width, spike->getContentSize().height));
			
			// set movement
			auto moveBy = MoveBy::create(1.f, Vec2(.0f, -spike->getContentSize().height * .8f));
			auto moveByRev = MoveBy::create(0.5f, Vec2(.0f, spike->getContentSize().height * .8f));

			auto repeatForever = RepeatForever::create(Sequence::create(moveBy, moveByRev, DelayTime::create(1.f), nullptr));
			obstacleNode->runAction(repeatForever);
		}
			break;
		case TAG_WEIGHT:
			break;
	}

#ifdef _DEBUG
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
	for (auto obs : _obstacles){
		if (obs->getName() == "lethal"){
			
			// lethal objects have slightly smaller bounding box for easier difficulty 
			auto obsRect = obs->getBoundingBox();
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

			if (obsRect.intersectsRect(playerBoundingBox)){

				// change sprite to dead
				_playerCharacter->stopAllActions();
				_playerCharacter->setSpriteFrame(Sprite::create("PNG/Enemies/slimePurple_dead.png")->getSpriteFrame());
				_playerCharacter->setFlippedX(true);
				_playerCharacter->setFlippedY(true);
				*_gameState = GAME_STATE::OVER;

				this->pause();
				gameOverSequence();


				return;
			}
			// 		if (obsRect.intersectsRect(playerRect)){
			// 
			// 			auto left = MAX(obsRect.getMinX(), playerRect.getMinX());
			// 			auto right = MIN(obsRect.getMaxX(), playerRect.getMaxX());
			// 			auto bottom = MAX(obsRect.getMinY(), playerRect.getMinY());
			// 			auto top = MIN(obsRect.getMaxY(), playerRect.getMaxY());
			// 
			// 			auto width = right - left;
			// 			auto height = top - bottom;
			// 
			// 			auto area = width * height;
			// 
			// 			// dies only if intersection size if larger than some number
			// 			CCLOG("intersected area: %f", area);
			// 			// 2% of player size
			// 			if (area > _playerSize * .2f){
			// 
			// 				// change sprite to dead
			// 				_playerCharacter->stopAllActions();
			// 				_playerCharacter->setSpriteFrame(Sprite::create("PNG/Enemies/slimePurple_dead.png")->getSpriteFrame());
			// 				_playerCharacter->setFlippedX(true);
			// 				_playerCharacter->setFlippedY(true);
			// 				*_gameState = GAME_STATE::OVER;
			// 
			// 				this->pause();
			// 				gameOverSequence();
			// 			}
			// 
			// 			break;
			// 		}
		}

		// non-lethal obstacles
		else{
			auto obsRect = obs->getBoundingBox();
			auto playerBoundingBox = _playerCharacter->getBoundingBox();


			// 10% off from left
			playerBoundingBox.origin += Vec2(playerBoundingBox.size.width * .1f, 0.f);
			// 80% of the original bounding box
			playerBoundingBox.size = Vec2(playerBoundingBox.size.width * .8f, playerBoundingBox.size.height);

			// if collides
			if (obsRect.intersectsRect(playerBoundingBox)){

				// it pushes player when collided on side (left only)

				// side way, slide
				if (obsRect.getMinX() > _lastPosition.x && (_reverseFall > 0 ? _lastPosition.y + _playerCharacter->getContentSize().height > obsRect.getMinY() : _lastPosition.y < obsRect.getMaxY())){

					_playerCharacter->setPositionX(obsRect.getMinX() - _playerCharacter->getContentSize().width * .9f);
				}
				else{
					// player can stand on non lethal obstacles.

					// colliding from top
					_fallSpeed = 0.f;
					_playerCharacter->setMidAir(false);
					_playerCharacter->setPositionY((_reverseFall > 0) ? obsRect.getMinY() - _playerCharacter->getContentSize().height : obsRect.getMaxY());

				}


				return;


			}
		}
	}
}
