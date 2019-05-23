//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Renderer/AnimTypes.hpp"

class SpriteSheet;
class SpriteDefenition;

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
