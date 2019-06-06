#pragma once

//------------------------------------------------------------------------------------------------------------------------------
enum SpriteAnimPlaybackType
{
	SPRITE_ANIM_PLAYBACK_ONCE,		// for 5-frame anim, plays 0,1,2,3,4,4,4,4,4,4,4,4,4,4,4...
	SPRITE_ANIM_PLAYBACK_LOOP,		// for 5-frame anim, plays 0,1,2,3,4,0,1,2,3,4,0,1,2,3,4...
	SPRITE_ANIM_PLAYBACK_PINGPONG,	// for 5-frame anim, plays 0,1,2,3,4,3,2,1,0,1,2,3,4,3,2...
	NUM_SPRITE_ANIM_PLAYBACK_TYPES
};

enum eAnimationType
{
	ANIMATION_IDLE,
	ANIMATION_WALK,
	ANIMATION_ATTACK,
	ANIMATION_DIE,

	ANIMATION_COUNT,
};