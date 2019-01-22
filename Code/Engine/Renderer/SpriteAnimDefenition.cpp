#include "Engine/Renderer/SpriteAnimDefenition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefenition.hpp"


SpriteAnimDefenition::SpriteAnimDefenition( const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimPlaybackType playbackType/*=SPRITE_ANIM_PLAYBACK_LOOP */ )
	: m_spriteSheet (sheet)
{
	m_startSpriteIndex = startSpriteIndex;
	m_endSpriteIndex = endSpriteIndex;
	m_durationSeconds = durationSeconds;
	m_playbackType = playbackType;
}

const SpriteDefenition& SpriteAnimDefenition::GetSpriteDefAtTime( float seconds ) const
{
	switch( m_playbackType )
	{
	case SPRITE_ANIM_PLAYBACK_ONCE:
	return GetSpriteDefAtTime_Once(seconds);
	break;
	case SPRITE_ANIM_PLAYBACK_LOOP:
	return GetSpriteDefAtTime_Loop(seconds);
	break;
	case SPRITE_ANIM_PLAYBACK_PINGPONG:
	return GetSpriteDefAtTime_PingPong(seconds);
	break;
	}
	
	return m_spriteSheet.GetSpriteDef(0);
}


const SpriteDefenition& SpriteAnimDefenition::GetSpriteDefAtTime_Once( float seconds ) const
{
	//Get total number of frames
	int numFrames = (m_endSpriteIndex - m_startSpriteIndex) + 1;
	float frameDuration = m_durationSeconds / numFrames;

	//Get the animation frame number
	int animFrameNum;
	if(seconds < m_durationSeconds)
	{
		animFrameNum = (int)floorf(seconds / frameDuration);
	}
	else
	{
		animFrameNum = m_endSpriteIndex;
	}

	return m_spriteSheet.GetSpriteDef(animFrameNum);
}


const SpriteDefenition& SpriteAnimDefenition::GetSpriteDefAtTime_Loop( float seconds ) const
{
	//Get total number of frames
	int numFrames = (m_endSpriteIndex - m_startSpriteIndex) + 1;
	float frameDuration = m_durationSeconds / numFrames;

	//Get the animation frame number
	float timeInCycle = fmodf(seconds, m_durationSeconds);
	int animFrameNum = (int)floorf(timeInCycle / frameDuration);

	return m_spriteSheet.GetSpriteDef(animFrameNum + m_startSpriteIndex);
}


const SpriteDefenition& SpriteAnimDefenition::GetSpriteDefAtTime_PingPong( float seconds ) const
{
	//Get number of frames and each frame duration
	int numFrames = (m_endSpriteIndex - m_startSpriteIndex) * 2;
	int numBaseFrames = m_endSpriteIndex - m_startSpriteIndex + 1;
	float frameDuration = m_durationSeconds / numFrames;

	//Get time in Cycle and animation frame number at that time
	float timeInCycle = fmodf(seconds, m_durationSeconds);
	int animFrameNum = (int)floorf(timeInCycle / frameDuration);

	if(animFrameNum < numBaseFrames)
	{
		return 	m_spriteSheet.GetSpriteDef(animFrameNum + m_startSpriteIndex);
	}
	else
	{
		int reverseIndexFromEnd = numBaseFrames - animFrameNum;
		return m_spriteSheet.GetSpriteDef(reverseIndexFromEnd + m_endSpriteIndex);
	}
}

