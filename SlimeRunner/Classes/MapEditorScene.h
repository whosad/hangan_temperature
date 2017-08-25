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
	void initBGandPlatform();

	void update(float delta) override;



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

	cocos2d::Size _visibleSize;
};

#endif //__MAP_EDITOR_SCENE_H__
