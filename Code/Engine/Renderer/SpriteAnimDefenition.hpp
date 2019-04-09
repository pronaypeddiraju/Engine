#pragma once
//------------------------------------------------------------------------------------------------------------------------------
class SpriteSheet;
class SpriteDefenition;

//------------------------------------------------------------------------------------------------------------------------------
enum SpriteAnimPlaybackType
{
	SPRITE_ANIM_PLAYBACK_ONCE,		// for 5-frame anim, plays 0,1,2,3,4,4,4,4,4,4,4,4,4,4,4...
	SPRITE_ANIM_PLAYBACK_LOOP,		// for 5-frame anim, plays 0,1,2,3,4,0,1,2,3,4,0,1,2,3,4...
	SPRITE_ANIM_PLAYBACK_PINGPONG,	// for 5-frame anim, plays 0,1,2,3,4,3,2,1,0,1,2,3,4,3,2...
	NUM_SPRITE_ANIM_PLAYBACK_TYPES
};

//------------------------------------------------------------------------------------------------------------------------------
class SpriteAnimDefenition
{
public:
	SpriteAnimDefenition( const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex,
		float durationSeconds, SpriteAnimPlaybackType playbackType=SPRITE_ANIM_PLAYBACK_LOOP );

	const SpriteDefenition& GetSpriteDefAtTime( float seconds ) const;

private:
	const SpriteSheet&			m_spriteSheet;
	int							m_startSpriteIndex = -1;
	int							m_endSpriteIndex = -1;
	float						m_durationSeconds = 1.f;
	SpriteAnimPlaybackType		m_playbackType = SPRITE_ANIM_PLAYBACK_LOOP;

	const SpriteDefenition& GetSpriteDefAtTime_Once(float seconds) const;
	const SpriteDefenition& GetSpriteDefAtTime_Loop(float seconds) const;
	const SpriteDefenition& GetSpriteDefAtTime_PingPong(float seconds) const;
};
