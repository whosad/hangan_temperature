#ifndef __TITLE_SCENE_H__
#define __TITLE_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "FirebaseHelper.h"

#include "firebase/admob.h"
#include "firebase/admob/types.h"
#include "firebase/app.h"
#include "firebase/future.h"
#include "firebase/admob/banner_view.h"

#include <android/log.h>
#include <jni.h>
#include "platform/android/jni/JniHelper.h"

#endif

class GameScene;

class TitleScene : public cocos2d::Scene
{
    // Methods
public:
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

	~TitleScene();
#endif
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

    // ���۹�ư ��ġ �ݹ�
    void buttonTouchEvent(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);

    // stage selection
    void setupStageSelctionMenu();

    // back button callback
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* e);

    // hide selection menu and show title and start button
    void hideSelectionMenu();

    // stage touch event
    bool onStageTouchBegan(cocos2d::Touch* touch, cocos2d::Event* e);

    // Variables
public:

private:
    cocos2d::Size _visibleSize;

    const float _SCROLLING_SPEED = 2.f;
    cocos2d::Vector<cocos2d::Sprite*> _backgroundVector;

    // title label
    cocos2d::Label* _titleLabel;

    // start button and its child label
    cocos2d::ui::Button* _startButton;
    cocos2d::Label* _startButtonLabel;

    // stage selection menu
    cocos2d::Node* _stageSelectionMenu = nullptr;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

	firebase::admob::BannerView* banner_view;

#endif

};

#endif //__TITLE_SCENE_H__
