#include "Engine/Renderer/SpriteDefenition.hpp"
#include "Engine/Renderer/TextureView.hpp"

//------------------------------------------------------------------------------------------------------------------------------
SpriteDefenition::SpriteDefenition(TextureView* textureView, const Vec2& minUVs, const Vec2& maxUVs)
{
	m_texture = textureView;
	m_minUVs = minUVs;
	m_maxUVs = maxUVs;
}

//------------------------------------------------------------------------------------------------------------------------------
SpriteDefenition::SpriteDefenition(const SpriteDefenition& defenition, const Vec2& pivot)
{
	m_texture = defenition.m_texture;
	m_pivot = pivot;
	m_minUVs = defenition.m_minUVs;
	m_maxUVs = defenition.m_maxUVs;
}

//------------------------------------------------------------------------------------------------------------------------------
SpriteDefenition::~SpriteDefenition()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void SpriteDefenition::SetPivot(const Vec2& pivot)
{
	m_pivot = pivot;
}

//------------------------------------------------------------------------------------------------------------------------------
void SpriteDefenition::SetUVs(const Vec2& minUVs, const Vec2& maxUVs)
{
	m_minUVs = minUVs;
	m_maxUVs = maxUVs;
}

//------------------------------------------------------------------------------------------------------------------------------
void SpriteDefenition::GetUVs( Vec2& minUVs, Vec2& maxUVs ) const
{
	minUVs = m_minUVs;
	maxUVs = m_maxUVs;
}

//------------------------------------------------------------------------------------------------------------------------------
TextureView* SpriteDefenition::GetTexture() const
{
	return m_texture;
}
