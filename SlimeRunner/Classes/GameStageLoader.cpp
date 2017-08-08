#include "GameStageLoader.h"

std::vector<std::pair<float, OBSTACLE_TYPE>> GameStageLoader::loadStage(int stageNum)
{
	std::vector<std::pair<float, OBSTACLE_TYPE>> returnMe;

	// stage file path
	std::stringstream ss;
	ss << "STAGES/stage" << stageNum;
	auto stageFilePath = ss.str();



#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

	auto file = FileUtils::getInstance()->getStringFromFile(ss.str());
	std::istringstream stageFile(file);


#else
	// file io
	std::ifstream stageFile(stageFilePath);

#endif


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if(file.length() <= 0){
		CCLOG("stage file  %s failed to open", stageFilePath.c_str());

		return returnMe;
	}

#else
	// error check
	if (!stageFile.is_open()){
		CCLOG("stage file  %s failed to open", stageFilePath.c_str());


		stageFile.close();

		return returnMe;
	}

#endif

	// parse each line
	std::string line;
	while (getline(stageFile, line)){

		// skip empty or commented line
		if (line.length() <= 0)            continue;
		else if (line[0] == '#')            continue;

		// trim white spaces
		line.erase(std::remove_if(line.begin(), line.end(), [](char c){return std::isspace(c); }), line.end());

		// tokenize and parse 
		std::pair<float, OBSTACLE_TYPE> intervalAndType(-1.f, NO_OBSTACLE);

		// only read first two
		auto pos = line.find(":");

		// first
		ss.str(std::string());
		ss.clear();
		ss.str(line.substr(0, pos));
		ss >> intervalAndType.first;
		//second
		ss.str(std::string());
		ss.clear();
		ss.str(line.substr(pos + 1, line.length() + 1));

		auto key = ss.str();

		if (key == "boxB"){
			intervalAndType.second = TAG_BOX_B;
		}
		else if (key == "boxT"){
			intervalAndType.second = TAG_BOX_T;
		}
		else if (key == "stairB"){
			intervalAndType.second = TAG_STAIR_B;
		}
		else if (key == "stairT"){
			intervalAndType.second = TAG_STAIR_T;
		}
		else if (key == "sawB"){
			intervalAndType.second = TAG_SAW_B;
		}
		else if (key == "sawT"){
			intervalAndType.second = TAG_SAW_T;
		}
		else if (key == "spikeB"){
			intervalAndType.second = TAG_SPIKE_B;
		}
		else if (key == "spikeT"){
			intervalAndType.second = TAG_SPIKE_T;
		}
		else if (key == "weight"){
			intervalAndType.second = TAG_WEIGHT;
		}
		// add more
		else{
			CCLOG("obstacle key not found");
			continue;
		}


		CCLOG("%f : %d", intervalAndType.first, intervalAndType.second);
		// something went wrong?
		if (intervalAndType.first <= 0 && intervalAndType.second <= 0)
			continue;

		returnMe.push_back(intervalAndType);
	}

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)

	stageFile.close();
#endif

	return returnMe;
};