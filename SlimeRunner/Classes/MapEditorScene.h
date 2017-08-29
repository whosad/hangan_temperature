#ifndef __MAP_EDITOR_SCENE_H__
#define __MAP_EDITOR_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class MapEditorScene : public cocos2d::Scene
{

	// Methods
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(MapEditorScene);

private:
	
    void update(float delta) override;

    void initBGandPlatform();

    void initObsPanel();
       
    void onMouseDown(cocos2d::Event* e);
    void onMouseMove(cocos2d::Event* e);

private: 

	const int _maxPixel = 20000;
	enum DIRECTION{
		NONE,
		LEFT,
		RIGHT
	};
	DIRECTION _dir = NONE;

	float _cameraScrollSpeed = 13.f;
	float _scrollModifier = 1.f;

       // x coord of left edge of the screen ( + mouse pos to find out obs position)
       float _currentPosX;

	cocos2d::Size _visibleSize;

       cocos2d::Sprite* _obsPanel;

       cocos2d::Vector<cocos2d::Sprite*> _itemList;

       bool _isItemClicked;

       cocos2d::Sprite* _clickedItem;
};

#endif //__MAP_EDITOR_SCENE_H__
