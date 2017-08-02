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
    // ��ӹ��� ������Ʈ �Լ�
    void update(float dt) override;
    
    // ���ȭ�� �̹��� 2�� ��ġ �����ϰ� ��ũ�� �ǰ�
    void setupBackground();
    
    // Ÿ��Ʋ �̸� ����
    void setupTitle();

    // ���� ��ư ����
    void setupStartButton();



    // Variables
public:

private:
    cocos2d::Size _visibleSize;

    const float _SCROLLING_SPEED = 2.f;
    cocos2d::Vector<cocos2d::Sprite*> _backgroundVector;


};

#endif //__TITLE_SCENE_H__
