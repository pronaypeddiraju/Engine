//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"

class TextureView;

//------------------------------------------------------------------------------------------------------------------------------
class SpriteDefenition
{
public:
	explicit SpriteDefenition(TextureView* textureView, const Vec2& minUVs, const Vec2& maxUVs);
	explicit SpriteDefenition(const SpriteDefenition& defenition, const Vec2& pivot);
	~SpriteDefenition();

	void					SetPivot(const Vec2& pivot);
	void					SetUVs(const Vec2& minUVs, const Vec2& maxUVs);

	void					GetUVs(Vec2& minUVs, Vec2& maxUVs) const;
	TextureView*			GetTexture() const;
	inline const Vec2& 		GetPivot() const { return m_pivot; }

private:
	TextureView*		m_texture = nullptr;
	Vec2				m_minUVs = Vec2::ZERO;
	Vec2				m_maxUVs = Vec2::ONE;
	Vec2				m_pivot = Vec2::ZERO;
};