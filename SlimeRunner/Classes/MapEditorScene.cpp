#include "MapEditorScene.h"
#include <fstream>

USING_NS_CC;

Scene* MapEditorScene::createScene()
{
	return MapEditorScene::create();
}

// on "init" you need to initialize your instance
bool MapEditorScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}


	_visibleSize = Director::getInstance()->getVisibleSize();


	_isItemClicked = false;
	_lCtrlDown = false;
	_zDown = false;
	_isTop = false;

	_multiple = 128.f;

	this->scheduleUpdate();

	// init background and top/bottom platform
	initBGandPlatform();

	// setup obstacle panel
	initObsPanel();

	// camera moves left to right
	auto keyListener = EventListenerKeyboard::create();
	auto dir = &_dir;
	auto mod = &_scrollModifier;
	auto mul = &_multiple;
	auto obsVec = &_obstacles;
	auto lctrl = &_lCtrlDown;
	auto z = &_zDown;

	keyListener->onKeyPressed = [dir, mod, mul, obsVec, lctrl, z](EventKeyboard::KeyCode keyCode, Event* event){
		if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW){
			*dir = LEFT;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW){
			*dir = RIGHT;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_SHIFT){
			*mod = 8.f;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_SHIFT){
			*mul = 8.f;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_CTRL){
			*lctrl = true;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_Z){
			*z = true;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_ENTER){
			CCLOG("saving current stage");

			int pixelsPassed = 0;

			// sort first?
			sort(obsVec->begin(), obsVec->end(), [](Node* a, Node* b){
				return a->getPositionX() < b->getPositionX();
			});


			// write to file
			std::ofstream myfile("STAGES/stageX");

			if (myfile.is_open())
			{
				myfile << "## Stage made with maptool\n\n\n";

				for(auto obs : *obsVec){

					// min = _visibleSize.width + 50.f
					int pos = obs->getPositionX() - pixelsPassed;
					pixelsPassed += pos;
					auto name = obs->getName();
					CCLOG("%d : %s", pos, name.c_str());

					myfile << pos << " : " << name << std::endl;
				}

				myfile.close();

			}

		}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		// back button for android
		else if (keyCode == EventKeyboard::KeyCode::KEY_BACK){
#else
		// backspace for windows debugging
		else if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE){
#endif
			Director::getInstance()->popScene();
		}
	};
	keyListener->onKeyReleased = [dir, mod, mul, lctrl, z](EventKeyboard::KeyCode keyCode, Event* event){
		if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW){
			*dir = NONE;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_SHIFT){
			*mod = 1.f;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_SHIFT){
			*mul = 128.f;
		}

		else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_CTRL){
			*lctrl = false;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_Z){
			*z = false;
		}
	};


	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);


	// endzone
	auto box = DrawNode::create();
	box->drawSolidRect(Vec2(_maxPixel, 0.f), Vec2(_maxPixel + 1000.f, _visibleSize.height), Color4F::RED);
	box->setLineWidth(4);
	this->addChild(box);


	// current pos
	_currentPosX = 0.f;

	return true;
	}

void MapEditorScene::initBGandPlatform()
{
	// bg
	auto bg = Sprite::create("PNG/Backgrounds/blue_grass.png");
	auto numOfBgs = _maxPixel / (int)bg->getContentSize().width + 1;

	for (int i = 0; i < numOfBgs; i++){

		auto clone = Sprite::createWithSpriteFrame(bg->getSpriteFrame());
		clone->setPositionX(i * bg->getContentSize().width);
		clone->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		this->addChild(clone);

	}

	auto platform = Sprite::create("PNG/Ground/grass/grassHalf_mid.png");
	auto numOfTiles = _maxPixel / (int)platform->getContentSize().width + 1;

	for (int i = 0; i < numOfTiles; i++){

		auto clone = Sprite::createWithSpriteFrame(platform->getSpriteFrame());
		clone->setPositionX(i * platform->getContentSize().width);
		clone->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		this->addChild(clone);

		auto clone1 = Sprite::createWithSpriteFrame(platform->getSpriteFrame());
		clone1->setPositionX(i * platform->getContentSize().width);
		clone1->setPositionY(_visibleSize.height - platform->getContentSize().height);
		clone1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		clone1->setFlippedY(true);
		this->addChild(clone1);

	}


}

void MapEditorScene::update(float delta)
{
	if (_dir == LEFT){
		this->getDefaultCamera()->setPositionX(MAX(_visibleSize.width * .5f, this->getDefaultCamera()->getPositionX() - _cameraScrollSpeed * _scrollModifier));

		_currentPosX = this->getDefaultCamera()->getPositionX() - _visibleSize.width * .5f;
	}
	else if (_dir == RIGHT){
		this->getDefaultCamera()->setPositionX(MIN(_maxPixel + 200.f, this->getDefaultCamera()->getPositionX() + _cameraScrollSpeed * _scrollModifier));

		_currentPosX = this->getDefaultCamera()->getPositionX() - _visibleSize.width * .5f;
	}

	if (_zDown && _lCtrlDown){
		_zDown = false;
		if (_obstacles.size() > 0){
			_obstacles.back()->removeFromParentAndCleanup(true);
			_obstacles.popBack();
		}

	}

	// follow camera
	_obsPanel->setPositionX(_currentPosX + _visibleSize.width);
}

void MapEditorScene::initObsPanel()
{
	_obsPanel = Sprite::create("PNG/rPanel.png");
	_obsPanel->setPosition(_visibleSize.width, _visibleSize.height * .5f);
	_obsPanel->setOpacity(150);
	_obsPanel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
	_obsPanel->setColor(Color3B::GREEN);
	this->addChild(_obsPanel);

	// items
	_itemList.pushBack(Sprite::create("PNG/box.png"));
	_itemList.back()->setName("sbox");
	
	_itemList.pushBack(Sprite::create("PNG/3box.png"));
	_itemList.back()->setName("box");

	_itemList.pushBack(Sprite::create("PNG/stair.png"));
	_itemList.back()->setName("stair");

	_itemList.pushBack(Sprite::create("PNG/saw.png"));

	_itemList.back()->setName("saw");

	_itemList.pushBack(Sprite::create("PNG/spike.png"));
	_itemList.back()->setName("spike");

	// set positions
	for (int i = 0; i < 5; i++){
		int col = i % 2;
		int row = i / 2;
		_itemList.at(i)->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		_itemList.at(i)->setPosition(Vec2(35.f + (col * (64.f + 20.f)), _obsPanel->getContentSize().height - 35.f - row *(64.f + 20.f)));

		_obsPanel->addChild(_itemList.at(i));
	}



	// mouse events
	auto mouseEventListener = EventListenerMouse::create();
	mouseEventListener->onMouseDown = CC_CALLBACK_1(MapEditorScene::onMouseDown, this);
	mouseEventListener->onMouseMove = CC_CALLBACK_1(MapEditorScene::onMouseMove, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseEventListener, this);

}

void MapEditorScene::onMouseDown(cocos2d::Event* event){
	EventMouse* e = (EventMouse*)event;

	
	// no items selected
	if (!_isItemClicked){
		// check item selection
		for (int i = 0; i < _itemList.size(); i++){
			auto iconLocation = _obsPanel->convertToWorldSpace(_itemList.at(i)->getPosition());	// bottom left coord
			iconLocation.y -= _itemList.at(i)->getContentSize().height; // anchor is top left
			iconLocation.x -= _currentPosX; // subtract camera postion
			auto boundingBox = Rect(iconLocation.x, iconLocation.y, _itemList.at(i)->getContentSize().width, _itemList.at(i)->getContentSize().height);
			auto locInView = e->getLocationInView();
			if (boundingBox.containsPoint(locInView)){

				CCLOG("hi %s", _itemList.at(i)->getName().c_str());

				_clickedItem = Node::create();
				_clickedItem->setPosition(e->getLocationInView());
				_clickedItem->setPositionX(_clickedItem->getPositionX() + _currentPosX);
				_clickedItem->setCascadeOpacityEnabled(true);
				_clickedItem->setOpacity(200);
				_clickedItem->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
				
				_clickedItem->setTag(64);

				auto name = _itemList.at(i)->getName();
				_clickedItem->setName(name);

				if (name == "sbox"){
					auto box1 = Sprite::create("PNG/Tiles/boxCrate_double.png");
					box1->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
					_clickedItem->addChild(box1);
				}
				else if (name == "box"){
					// 3 boxes
					auto box1 = Sprite::create("PNG/Tiles/boxCrate_double.png");
					auto box2 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());
					auto box3 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());

					// manual midpoiot
					box1->setPosition(0.f, 128.f);
					box3->setPosition(0.f, -128.f);

					box1->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
					box2->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
					box3->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);


					_clickedItem->addChild(box1);
					_clickedItem->addChild(box2);
					_clickedItem->addChild(box3);
				}
				else if (name == "stair"){

					auto box1 = Sprite::create("PNG/Tiles/boxCrate_double.png");
					auto box2 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());
					auto box3 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());

					box1->setPosition(256.f , 128.f);
					box2->setPosition(256.f , 0.f);
					box3->setPosition(256.f , -128.f);

					box1->setName("left1");
					box2->setName("left2");
					box3->setName("left3");

					box1->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
					box2->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
					box3->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);

					_clickedItem->addChild(box1);
					_clickedItem->addChild(box2);
					_clickedItem->addChild(box3);

					// add 3 obstacle nodes

					// 2 boxes
					auto box4 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());
					auto box5 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());


					box4->setPosition(128.f, 0.f);
					box5->setPosition(128.f, -128.f);

					box4->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
					box5->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);

					box4->setName("midup1");
					box5->setName("midup2");

					_clickedItem->addChild(box4);
					_clickedItem->addChild(box5);

					// 1 box

					auto box6 = Sprite::createWithSpriteFrame(box1->getSpriteFrame());
					
					box6->setName("up2");

					box6->setPosition(0.f, -128.f);

					box6->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);

					_clickedItem->addChild(box6);
					_clickedItem->setTag(64 + 128);


				}
				else if (name == "saw"){
					auto saw = Sprite::create("PNG/Enemies/sawHalf.png");
					saw->setName("sprite");

					saw->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);

					_clickedItem->addChild(saw);

				}
				else if (name == "spike"){
					auto spike = Sprite::create("PNG/Tiles/spikes.png");
					spike->setName("sprite");

					spike->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);

					_clickedItem->addChild(spike);

				}




				this->addChild(_clickedItem);
				_isItemClicked = true;

				break;
			}
		}
	}
	// has item selected
	else{
		// r button to cancel selection
		if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT){

			_clickedItem->removeAllChildrenWithCleanup(true);
			_clickedItem->removeFromParentAndCleanup(true);
			_isItemClicked = false;

		}
		// l button to place it on the scene
		else{
			
			auto panelLocation = this->convertToWorldSpace(_obsPanel->getPosition());	// mid right pivot
			auto bb = Rect(panelLocation.x - _obsPanel->getContentSize().width,
				panelLocation.y - _obsPanel->getContentSize().height * .5f,
				_obsPanel->getContentSize().width,
				_obsPanel->getContentSize().height
				);
			if (!bb.containsPoint(e->getLocationInView())){
				auto newNode = cloneNodeWithSprites(_clickedItem);

				_obstacles.pushBack(newNode);

				this->addChild(newNode);

			}

		}

	}

}

void MapEditorScene::onMouseMove(cocos2d::Event* event){
	EventMouse* e = (EventMouse*)event;
	if (_isItemClicked){
		auto cursorPos = e->getLocationInView();
		cursorPos.x += _currentPosX;

		cursorPos.x = cursorPos.x + _multiple / 2;
		cursorPos.x -= (int)cursorPos.x % _multiple;


		_isTop = (cursorPos.y > _visibleSize.height * .5f) ? true : false;

		auto name = _clickedItem->getName();

		float top = _visibleSize.height - 64.f; // 720  656
		float bot = 64.f;						// 64

		// snap please
		if (name == "sbox"){
			cursorPos.y = _isTop ? top - 64.f : bot + 64.f;
		}
		else if (name == "box"){
			cursorPos.y = _isTop ? top - 64.f - 128.f : bot + 64.f + 128.f;
		}
		else if (name == "stair"){
			cursorPos.y = _isTop ? top - 64.f - 128.f : bot + 64.f + 128.f;

			auto box1 = _clickedItem->getChildByName<Sprite*>("left1");
			auto box2 = _clickedItem->getChildByName<Sprite*>("left2");
			auto box3 = _clickedItem->getChildByName<Sprite*>("left3");
			auto box4 = _clickedItem->getChildByName<Sprite*>("midup1");
			auto box5 = _clickedItem->getChildByName<Sprite*>("midup2");
			auto box6 = _clickedItem->getChildByName<Sprite*>("up2");

			if (_isTop){
				box1->setPosition(0.f, 128.f);
				box2->setPosition(0.f, 0.f);
				box3->setPosition(0.f, -128.f);

				box4->setPosition(128.f, 0.f);
				box5->setPosition(128.f, +128.f);

				box6->setPosition(256.f, +128.f);			
			}
			else{
				box1->setPosition(256.f, 128.f);
				box2->setPosition(256.f, 0.f);
				box3->setPosition(256.f, -128.f);

				box4->setPosition(128.f, 0.f);
				box5->setPosition(128.f, -128.f);

				box6->setPosition(0.f, -128.f);
			}
		
		}
		else if (name == "saw"){
			cursorPos.y = _isTop ? top - 32.f : bot + 32.f;

			_clickedItem->getChildByName<Sprite*>("sprite")->setFlippedY(_isTop);

		}
		else if (name == "spike"){
			cursorPos.y = _isTop ? top - 32.f : bot + 32.f;

			_clickedItem->getChildByName<Sprite*>("sprite")->setFlippedY(_isTop);
		}


		CCLOG("%f", cursorPos.x);
		_clickedItem->setPosition(cursorPos);
	}
}

cocos2d::Node* MapEditorScene::cloneNodeWithSprites(cocos2d::Node* _clickedItem)
{
	auto newNode = Node::create();

	newNode->setPosition(_clickedItem->getPosition());
	newNode->setAnchorPoint(_clickedItem->getAnchorPoint());

	//newNode->setTag(_clickedItem->getTag());
	auto newName = _clickedItem->getName();
	newName.append(_isTop ? "T" : "B");
	newNode->setName(newName);

	auto children = _clickedItem->getChildren();
	for (auto child : children){
		auto childSpr = (Sprite*)child;
		auto spr = Sprite::createWithSpriteFrame(childSpr->getSpriteFrame());
		spr->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
		spr->setPosition(childSpr->getPosition());
		spr->setFlippedY(childSpr->isFlippedY());
		newNode->addChild(spr);
	}

	return newNode;
}
