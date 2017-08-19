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
               _playerHealth -= 1;
               _skillGauge = MAX(_skillGauge - 20.f, 0.f);
           }
       };

       // check if dead
       bool isDead(){return _playerHealth <= 0 ? true: false; };

       // run blinking animation when hit
       void runBlink();

       int& getHealth(){ return _playerHealth; };
       float& getGauge(){ return _skillGauge; };

       // increase gauge every second
       void increaseGauge(float increament = .05f){ _skillGauge = MIN(_skillGauge + increament, 100.f); };

private:

    void initOptions();



    // Variables
public:


private:

    bool _isInvincible;
    bool _isMidAir ;
	cocos2d::RepeatForever* _moveAnimate;

       // health
       int _playerHealth;

       // gauge for skill
       float _skillGauge;

};

#endif //__PLAYER_CHARACTER_H__
