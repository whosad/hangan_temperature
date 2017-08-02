#ifndef __TITLE_SCENE_H__
#define __TITLE_SCENE_H__

#include "cocos2d.h"

class GameScene;

class TitleScene : public cocos2d::Scene
{
    // Methods
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(TitleScene);

private:
    // 상속받은 업데이트 함수
    void update(float dt) override;
    
    // 배경화면 이미지 2개 위치 설정하고 스크롤 되게
    void setupBackground();
    
    // 타이틀 이름 설정
    void setupTitle();

    // 시작 버튼 설정
    void setupStartButton();



    // Variables
public:

private:
    cocos2d::Size _visibleSize;

    const float _SCROLLING_SPEED = 2.f;
    cocos2d::Vector<cocos2d::Sprite*> _backgroundVector;


};

#endif //__TITLE_SCENE_H__
