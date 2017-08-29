#include "TitleScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"
#include "MapEditorScene.h"

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
    if(!Scene::init())
    {
        return false;
    }

#ifdef COCOS2D_DEBUG

	UserDefault::getInstance()->setIntegerForKey("unlockedStage", 2);
	FileUtils::getInstance()->writeStringToFile("FindMeOnAndroid\n", "STAGES/aa.txt");

	// map editor
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event){
		if (keyCode == EventKeyboard::KeyCode::KEY_SPACE){
			Director::getInstance()->pushScene(MapEditorScene::createScene());
		}
	};


	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

#endif

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
    touchEvent->onKeyReleased = CC_CALLBACK_2(TitleScene::onKeyReleased, this);
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
    _titleLabel = Label::createWithTTF("SLIME RUNNER", "FONTS/kenpixel_blocks.ttf", 85.f, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
    _titleLabel->setColor(Color3B(30, 220, 30));
    _titleLabel->setPosition(_visibleSize.width / 2, _visibleSize.height * .75f);
    _titleLabel->enableShadow(Color4B::BLACK, Size(3, -3));

    auto scaleEnlarge = ScaleTo::create(.75f, 1.05f);
    auto scaleBackToNormal = ScaleTo::create(.75f, 1.f);
    auto sequence = Sequence::create(scaleEnlarge, scaleBackToNormal, nullptr);

    _titleLabel->runAction(RepeatForever::create(sequence));

    this->addChild(_titleLabel, 1);
}

void TitleScene::setupStartButton()
{
    // create button
    _startButton = ui::Button::create("PNG/UI_Pack/yellow_button02.png", "PNG/UI_Pack/yellow_button03.png");
    this->addChild(_startButton, 1);

    // set text
    _startButtonLabel = Label::createWithTTF("START", "FONTS/kenpixel_blocks.ttf", 28.f, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::TOP);
    _startButtonLabel->setColor(Color3B::BLACK);
    _startButtonLabel->setPosition(_startButton->getContentSize().width * .5f, _startButton->getContentSize().height *.5f);
    
    _startButton->setScale(1.5f);

    _startButton->addChild(_startButtonLabel, 0);

    // button touch event
    _startButton->addTouchEventListener(CC_CALLBACK_2(TitleScene::buttonTouchEvent, this));


    // set position
    _startButton->setPosition(Vec2(_visibleSize.width * .5f, _visibleSize.height * .25f));

}

void TitleScene::setupStageSelctionMenu()
{

    // disable and hide start button
    _startButton->pause();
    _startButton->setVisible(false);

    // hide title as well
    _titleLabel->setVisible(false);


    // show and resume if already created
    if(_stageSelectionMenu){
        _stageSelectionMenu->setVisible(true);
        _stageSelectionMenu->resume();

    }
    // create stage selection menu if not created already
    else{
        // create
        _stageSelectionMenu = Node::create();
    
        // icons for stages. grass, cactus, mushroom. they all use crey panel as background
        Vector<Sprite*> stagePanels;

        // duplicate panel
        stagePanels.pushBack(Sprite::create("PNG/UI_Pack/grey_panel_2x.png"));
        stagePanels.pushBack(Sprite::createWithSpriteFrame(stagePanels.front()->getSpriteFrame()));
        stagePanels.pushBack(Sprite::createWithSpriteFrame(stagePanels.front()->getSpriteFrame()));

        // add stage background as background
        auto bgSize = Sprite::create("PNG/Backgrounds/colored_grass.png")->getContentSize();  // to get contents size
        auto panelSize = stagePanels.front()->getContentSize();
        auto subRect = Rect(
             0.f,
            bgSize.height * .55f - panelSize.height * .8 * .5f,
            panelSize.width * .8f,
            panelSize.height * .8f
            );

        /************************************************************************/
        /*  지금은 스테이지 3개니까 상관 없는데 나중엔 갯수만큼 인풋 받게 제네릭하게 바꿔야댐            */
        /************************************************************************/

        // add background for icon
        stagePanels.at(0)->addChild(Sprite::create("PNG/Backgrounds/colored_grass.png", subRect));
        stagePanels.at(1)->addChild(Sprite::create("PNG/Backgrounds/colored_desert.png", subRect));
        stagePanels.at(2)->addChild(Sprite::create("PNG/Backgrounds/colored_shroom.png", subRect));

        // new bg size
        bgSize = stagePanels.front()->getChildren().front()->getContentSize();
        CCLOG("new bg  size %f, %f", bgSize.width, bgSize.height);

        // set opacity for bgs..?
        for(auto panel : stagePanels){
            panel->getChildren().front()->setOpacity(195);
        }

        // add icon as child
        stagePanels.at(0)->addChild(Sprite::create("PNG/Tiles/grass.png"));
        stagePanels.at(1)->addChild(Sprite::create("PNG/Tiles/cactus.png"));
        stagePanels.at(2)->addChild(Sprite::create("PNG/Tiles/mushroomBrown.png"));


        // set color for disabled stages
        // read from user default?
        auto unlockedStage = UserDefault::getInstance()->getIntegerForKey("unlockedStage", 0);

        // unlocked stage cannot exeed number of stages
        unlockedStage = MIN(unlockedStage, stagePanels.size() - 1);

        for(int i = stagePanels.size() - 1; i  > unlockedStage ; i--){
            // draw node for shading
            auto drawNode = DrawNode::create();
            drawNode->drawSolidRect(Vec2(-bgSize.width * .5f, -bgSize.height * .5f), Vec2(bgSize.width * .5f, bgSize.height * .5f), Color4F(.35f, .35f, .35f, .85f));
            drawNode->setName("disabledStage");
            stagePanels.at(i)->addChild(drawNode);
        }

        // add event listener for unlocked stages
        auto touchEvent = EventListenerTouchOneByOne::create();
        touchEvent->setSwallowTouches(true);
        touchEvent->onTouchBegan = CC_CALLBACK_2(TitleScene::onStageTouchBegan, this);

        // first stage always open
        this->_eventDispatcher->addEventListenerWithSceneGraphPriority(touchEvent, stagePanels.at(0));
        for(int i = 1; i <= unlockedStage; i++){
            this->_eventDispatcher->addEventListenerWithSceneGraphPriority(touchEvent->clone(), stagePanels.at(i));
        }

        // icon position
        auto labelCenterPos = Vec2(panelSize.width * .5f, panelSize.height * .5f);

        // i used for gapping stage panels
        int i = - stagePanels.size() / 2;

        for(auto panel : stagePanels){

            // set positon of children to the center
            auto children = panel->getChildren();
            for(auto child : children){
                child->setPosition(labelCenterPos);
            }

            // set position of panel as well
            panel->setPosition(i++ * panelSize.width * 1.15f ,0.f);

            // add panels to menu
            _stageSelectionMenu->addChild(panel);
        }

        _stageSelectionMenu->setPosition(_visibleSize.width *.5f, _visibleSize.height *.5f);
        this->addChild(_stageSelectionMenu, 1);
    }

}

void TitleScene::buttonTouchEvent(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
    switch(type){

        case ui::Widget::TouchEventType::ENDED:
            _startButtonLabel->setPositionY(_startButton->getContentSize().height *.5f);

            // display stage selction menu
            setupStageSelctionMenu();

            break;
        default:
            if(!_startButton->isHighlighted())
                _startButtonLabel->setPositionY(_startButton->getContentSize().height *.5f);
            else
                _startButtonLabel->setPositionY(_startButton->getContentSize().height *.5f - 2.f);
            break;
    }
}

void TitleScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* e)
{
    // quit application upon pressing back button
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // back button for android
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK){
#else
    // backspace for windows debugging
    if(keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE){
#endif
        
        // if stage selection is open, back to title label
        if(!_titleLabel->isVisible()){
            hideSelectionMenu();

            // terminate application
        } else{
            Director::getInstance()->end();
        }
    }
}

void TitleScene::hideSelectionMenu()
{
    // hide and pause selection menu
    _stageSelectionMenu->setVisible(false);
    _stageSelectionMenu->pause();

    // resume and show title and button
    _titleLabel->setVisible(true);

    _startButton->setVisible(true);
    _startButton->resume();
}

bool TitleScene::onStageTouchBegan(cocos2d::Touch* touch, cocos2d::Event* e)
{
    // find out which one is touched and proceed to selected stage
    auto panels = _stageSelectionMenu->getChildren();
    auto target = (Sprite*)(e->getCurrentTarget());

    auto locInNode = target->convertToNodeSpace(touch->getLocation());
    auto size = target->getContentSize();
    auto rect = Rect(0,0,size.width, size.height);

    if(rect.containsPoint(locInNode)){

        // find out which stage number has been selected
        int selectedStage = -1;
        for(int i = 0; i < panels.size(); i++){
            if((Sprite*)panels.at(i) == target){
                selectedStage = i;
                break;
            }
        }
        if(selectedStage < 0){
            CCLOG("Something weird has been touched..");
            return false;
        }

        // go to game scene with corresponding stage data
        auto gameScene = GameScene::create();

        // check if stage fails to load
        if(gameScene->setStage(selectedStage)){

            Director::getInstance()->replaceScene(TransitionFade::create(0.3f, gameScene, Color3B::BLACK));
            return true;

        }
        else{
            CCLOG("loading stage %d has failed", selectedStage);
            return false;
        }
    }   
    return false;
}
