#ifndef __GAME_STAGE_LOADER_H__
#define __GAME_STAGE_LOADER_H__

#include "cocos2d.h"

#include "GameState.h"

#include <fstream>
#include <cctype>

USING_NS_CC;




class GameStageLoader
{

public:

	// loads stage
	static std::vector<std::pair<float, OBSTACLE_TYPE>> loadStage(int stageNum);

};

#endif //__GAME_STAGE_LOADER_H__
