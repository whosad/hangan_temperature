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


	this->scheduleUpdate();

	// init background and top/bottom platform
	initBGandPlatform();

	// setup obstacle panel


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
	}
	else if (_dir == RIGHT){
		this->getDefaultCamera()->setPositionX(MIN(_maxPixel + 200.f, this->getDefaultCamera()->getPositionX() + _cameraScrollSpeed * _scrollModifier));
	}
}
