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

       // hit boolean
       bool isHit(){ return _isInvincible; };
       void setHit(bool hit){ 
           _isInvincible = hit;
           // lets handle hp here
           if(hit){
               _playerHealth -= 10.f;
           }
       };

       // check if dead
       bool isDead(){ if(_playerHealth <= 0.f) return true; return false; };

       // run blinking animation when hit
       void runBlink();

       const float& getHealth(){ return _playerHealth; };

private:

    void initOptions();



    // Variables
public:


private:
    bool _isInvincible;
    bool _isMidAir ;
	cocos2d::RepeatForever* _moveAnimate;

       float _playerHealth;

};

#endif //__PLAYER_CHARACTER_H__
