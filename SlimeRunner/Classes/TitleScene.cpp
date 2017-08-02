#include "TitleScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
#include "GameScene.h"

USING_NS_CC;

Scene* TitleScene::createScene()
{
    return TitleScene::create();
}

// on "init" you need to initialize your instance
bool TitleScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    
     _visibleSize = Director::getInstance()->getVisibleSize();
//     Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /*
    배경화면 말고 케릭터나 바닥 타일같은건 TexturePacker로 편집해서 다시 올리겠음
    */


    // 업데이트 루프 사용하게
    this->scheduleUpdate();

    // 스크롤링 하는 배경 만들어놓고
    setupBackground();

    // title scene에는 위에 제목
    setupTitle();

    // 아래에 시작버튼 만들기
    setupStartButton();

    // 백 버튼으로 종료
    auto touchEvent = EventListenerKeyboard::create();
    touchEvent->onKeyReleased = [](EventKeyboard::KeyCode keyCode, Event* e){
        // quit application upon pressing back button
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        // back button for android
        if(keyCode == EventKeyboard::KeyCode::KEY_BACK){
#else
        // backspace for windows debugging
        if(keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE){
#endif
            Director::getInstance()->end();
        }
    };
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(touchEvent, this);


    return true;
}

// 업데이트 게임 루프
void TitleScene::update(float dt)
{
    // iterate through sprites
    for(auto bg : _backgroundVector){
        // move sprites
        bg->setPositionX(bg->getPositionX() - _SCROLLING_SPEED);
        // if the position is completely off the screen, relocate it to the end of the other sprite
        if(bg->getPositionX() <= -bg->getContentSize().width){
            bg->setPositionX(bg->getPositionX() + bg->getContentSize().width * _backgroundVector.size()); // width * number of sprites
        }
    }
}

void TitleScene::setupBackground()
{
    // update 에서 화면 돌아가게 만들자

    // load initial image
    _backgroundVector.pushBack(Sprite::create("PNG/Backgrounds/blue_grass.png"));
    _backgroundVector.front()->setAnchorPoint(Vec2::ZERO);
    _backgroundVector.front()->setPosition(0.f, 0.f);
    this->addChild(_backgroundVector.front());

    // set number of duplicated background image using width and screen width
    // starts at 1 since we already have one at the beginning
    auto bgSize = ceil(_visibleSize.width / _backgroundVector.front()->getContentSize().width) + 1;
    for(int i = 1; i < bgSize; i++){
        // duplicate sprite
        _backgroundVector.pushBack(Sprite::createWithSpriteFrame(_backgroundVector.front()->getSpriteFrame()));
        // se anchor point to 0,0
        _backgroundVector.back()->setAnchorPoint(Vec2::ZERO);
        // set initial positions
        _backgroundVector.back()->setPosition(_backgroundVector.front()->getContentSize().width * i, 0.f);
        // add to scene
        this->addChild(_backgroundVector.back());
    }
}

void TitleScene::setupTitle()
{
    auto titleLabel = Label::createWithTTF("SLIME RUNNER", "FONTS/kenpixel_blocks.ttf", 85.f, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
    titleLabel->setColor(Color3B(30, 220, 30));
    titleLabel->setPosition(_visibleSize.width / 2, _visibleSize.height * .75f);
	titleLabel->enableShadow(Color4B::BLACK, Size(3,-3));

    auto scaleEnlarge = ScaleTo::create(.75f, 1.05f);
    auto scaleBackToNormal = ScaleTo::create(.75f, 1.f);
    auto sequence = Sequence::create(scaleEnlarge, scaleBackToNormal, nullptr);

    titleLabel->runAction(RepeatForever::create(sequence));

    this->addChild(titleLabel, 1);
}

void TitleScene::setupStartButton()
{
    // create button
    auto startButton = ui::Button::create("PNG/UI_PACK/yellow_button02.png", "PNG/UI_PACK/yellow_button03.png");

    // set text
    auto startButtonLabel = Label::createWithTTF("START", "FONTS/kenpixel_blocks.ttf", 28.f, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
    startButtonLabel->setColor(Color3B::BLACK);
    startButtonLabel->setPosition(startButton->getContentSize().width * .5f, startButton->getContentSize().height *.5f);

    startButton->addChild(startButtonLabel, 0);

    // button touch event
    startButton->addTouchEventListener([startButton, startButtonLabel](Ref* sender, 
        ui::Widget::TouchEventType type){
            switch(type){
                case ui::Widget::TouchEventType::ENDED:
                    startButtonLabel->setPositionY(startButton->getContentSize().height *.5f);
                    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameScene::create(), Color3B::BLACK));
                    break;
                default:
                    if(!startButton->isHighlighted())
                        startButtonLabel->setPositionY(startButton->getContentSize().height *.5f);
                    else
                        startButtonLabel->setPositionY(startButton->getContentSize().height *.5f - 2.f);
                    break;
            }
        }
    );

    // set position
    startButton->setPosition(Vec2(_visibleSize.width * .5f, _visibleSize.height * .25f));
    
    this->addChild(startButton, 1);
}