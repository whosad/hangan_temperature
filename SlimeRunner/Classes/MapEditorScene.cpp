#include "MapEditorScene.h"


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
    if(!Scene::init())
    {
        return false;
    }


	_visibleSize = Director::getInstance()->getVisibleSize();


       _isItemClicked = false;

	this->scheduleUpdate();

	// init background and top/bottom platform
	initBGandPlatform();

	// setup obstacle panel
       initObsPanel();

	// camera moves left to right
	auto keyListener = EventListenerKeyboard::create();
	auto dir = &_dir;
	auto mod = &_scrollModifier;
	keyListener->onKeyPressed = [dir, mod](EventKeyboard::KeyCode keyCode, Event* event){
		if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW){
			*dir = LEFT;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW){
			*dir = RIGHT;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_SHIFT){
			*mod = 8.f;
		}
	};
	keyListener->onKeyReleased = [dir, mod](EventKeyboard::KeyCode keyCode, Event* event){
		if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW){
			*dir = NONE;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_SHIFT){
			*mod = 1.f;
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
    _itemList.back()->setName("box");

    _itemList.pushBack(Sprite::create("PNG/3box.png"));
    _itemList.back()->setName("3box");
    
    _itemList.pushBack(Sprite::create("PNG/stair.png"));
    _itemList.back()->setName("stair");
    _itemList.pushBack(Sprite::create("PNG/saw.png"));

    _itemList.back()->setName("saw");

    _itemList.pushBack(Sprite::create("PNG/spike.png"));
    _itemList.back()->setName("spike");

    // set positions
    for(int i = 0; i < 5; i++){
        int col = i % 2;
        int row = i / 2;
        _itemList.at(i)->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        _itemList.at(i)->setPosition(Vec2(35.f + (col * (64.f + 20.f)), _obsPanel->getContentSize().height - 35.f - row *(64.f + 20.f)));

        _obsPanel->addChild(_itemList.at(i));
    }

    /*
              if (key == "boxB"){
              intervalAndType.second = TAG_BOX_B;
              }
              else if (key == "boxT"){
              intervalAndType.second = TAG_BOX_T;
              }
              else if (key == "stairB"){
              intervalAndType.second = TAG_STAIR_B;
              }
              else if (key == "stairT"){
              intervalAndType.second = TAG_STAIR_T;
              }
              else if (key == "sawB"){
              intervalAndType.second = TAG_SAW_B;
              }
              else if (key == "sawT"){
              intervalAndType.second = TAG_SAW_T;
              }
              else if (key == "spikeB"){
              intervalAndType.second = TAG_SPIKE_B;
              }
              else if (key == "spikeT"){
              intervalAndType.second = TAG_SPIKE_T;
              }
              else if (key == "weight"){
              intervalAndType.second = TAG_WEIGHT;
              }
    */
    

    // mouse events
    auto mouseEventListener = EventListenerMouse::create();
    mouseEventListener->onMouseDown = CC_CALLBACK_1(MapEditorScene::onMouseDown, this);
    mouseEventListener->onMouseMove = CC_CALLBACK_1(MapEditorScene::onMouseMove, this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseEventListener, this);

}

void MapEditorScene::onMouseDown(cocos2d::Event* event){
    EventMouse* e = (EventMouse*)event;


    // no items selected
    if(!_isItemClicked){
        // check item selection
        for(int i = 0; i < _itemList.size(); i++){
            auto iconLocation = _obsPanel->convertToWorldSpace(_itemList.at(i)->getPosition());	// bottom left coord
            iconLocation.y -= _itemList.at(i)->getContentSize().height; // anchor is top left
            iconLocation.x -= _currentPosX; // subtract camera postion
            auto boundingBox = Rect(iconLocation.x, iconLocation.y, _itemList.at(i)->getContentSize().width, _itemList.at(i)->getContentSize().height);
            auto locInView = e->getLocationInView();
            if(boundingBox.containsPoint(locInView)){

                CCLOG("hi %s", _itemList.at(i)->getName().c_str());

                _clickedItem = Sprite::createWithSpriteFrame(_itemList.at(i)->getSpriteFrame());
                _clickedItem->setPosition(e->getLocationInView());
                _clickedItem->setPositionX(_clickedItem->getPositionX() + _currentPosX);
                _clickedItem->setName(_itemList.at(i)->getName());
                this->addChild(_clickedItem, 3);
                _isItemClicked = true;

                break;
            }
        }
    }
    // has item selected
    else{

        // only cancel selection if clicked on left panel
        auto panelLocation = this->convertToWorldSpace(_obsPanel->getPosition());	// mid right pivot
        auto bb = Rect(panelLocation.x - _obsPanel->getContentSize().width,
                       panelLocation.y - _obsPanel->getContentSize().height * .5f,
                       _obsPanel->getContentSize().width,
                       _obsPanel->getContentSize().height
                       );
        if(!bb.containsPoint(e->getLocationInView())){
            this->removeChild(_clickedItem);
            _isItemClicked = false;
        }

    }

}

void MapEditorScene::onMouseMove(cocos2d::Event* event){
    EventMouse* e = (EventMouse*)event;
    if(_isItemClicked){
        _clickedItem->setPosition(e->getLocationInView());
        _clickedItem->setPositionX(_clickedItem->getPositionX() + _currentPosX);
    }
}