#include "GameUILayer.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

// on "init" you need to initialize your instance
bool GameUILayer::init()
{
    //////////////////////////////
    // 1. super init first
    if(!Node::init())
    {
        return false;
    }

    CCLOG("Game UI Layer loaded");

    return true;
}
