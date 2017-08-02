#ifndef __PLAYER_CHARACTER_H__
#define __PLAYER_CHARACTER_H__

#include "cocos2d.h"

class PlayerCharacter : public cocos2d::Sprite
{
    // Methods
public:

    static PlayerCharacter* create();

    bool isMidAir(){ return _isMidAir; };
    void setMidAir(bool midAir){ _isMidAir = midAir; };

private:

    void initOptions();



    // Variables
public:


private:
    cocos2d::RepeatForever* _moveAnimate;
    bool _isMidAir;

};

#endif //__PLAYER_CHARACTER_H__
