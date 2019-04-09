#include "Engine/Renderer/SpriteDefenition.hpp"

//------------------------------------------------------------------------------------------------------------------------------
SpriteDefenition::SpriteDefenition(const Vec2& minUVs, const Vec2& maxUVs)
	: m_uvAtBottomLeft(minUVs),
	m_uvAtTopright(maxUVs)
{
	
}

//------------------------------------------------------------------------------------------------------------------------------
SpriteDefenition::~SpriteDefenition()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void SpriteDefenition::GetUVs( Vec2& minUVs, Vec2& maxUVs )
{
	minUVs = m_uvAtBottomLeft;
	maxUVs = m_uvAtTopright;
}
