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
	void runMoveAnimation(){    
		// run repeatforever action
		this->runAction(_moveAnimate);
	};

private:

    void initOptions();



    // Variables
public:


private:
    bool _isMidAir;
	cocos2d::RepeatForever* _moveAnimate;

};

#endif //__PLAYER_CHARACTER_H__
