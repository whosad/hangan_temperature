#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

    enum GAME_STATE{
        PAUSED,
        PLAYING,
        OVER
    };

	// obstacle tags
	enum OBSTACLE_TYPE{
		NO_OBSTACLE,
		TAG_BOX_T,
		TAG_BOX_B,
		TAG_SINGLE_BOX_T,
		TAG_SINGLE_BOX_B,
		TAG_STAIR_T,
		TAG_STAIR_B,
		TAG_SAW_T,
		TAG_SAW_B,
		TAG_SPIKE_T,
		TAG_SPIKE_B,
		TAG_WEIGHT

	};


#endif //__GAME_STATE_H__
