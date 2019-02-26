#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

struct AABB2
{
public:
	AABB2();
	~AABB2();
	explicit AABB2(const Vec2& minBounds,const Vec2& maxBounds);
	explicit AABB2(const char* asText);

	Vec2 m_minBounds;
	Vec2 m_maxBounds;

	Vec2 m_center;

	//Accessors
	//AABB2			GetAABB2Bounds() const	{return AABB2(m_minBounds, m_maxBounds);}
	const Vec2&		GetBoxCenter() const;

	//Mutators
	void			SetFromText(const char* asText);

	Vec3			GetTopLeft() const;
	Vec3			GetTopRight() const;
	Vec3			GetBottomLeft() const;
	Vec3			GetBottomRight() const;

	void			Resize( const Vec2& newDimensions, const Vec2& pivotAnchorAlignment = Vec2::ALIGN_CENTERED );
	void			AlignWithinBox( const AABB2& box, const Vec2& alignment );
	AABB2			GetBoxWithin( const Vec2& dimensions, const Vec2& alignment ) const;

	AABB2			GetBoxAtLeft( float fractionOfWidth, 		float additionalWidth=0.f ) const;
	AABB2			GetBoxAtRight( float fractionOfWidth, 		float additionalWidth=0.f ) const;
	AABB2			GetBoxAtBottom( float fractionOfHeight, 		float additionalHeight=0.f ) const;
	AABB2			GetBoxAtTop( float fractionOfHeight, 		float additionalHeight=0.f ) const;

	AABB2			CarveBoxOffLeft( float fractionOfWidth, 		float additionalWidth=0.f );
	AABB2			CarveBoxOffRight( float fractionOfWidth, 		float additionalWidth=0.f );
	AABB2			CarveBoxOffBottom( float fractionOfHeight, 	float additionalHeight=0.f );
	AABB2			CarveBoxOffTop( float fractionOfHeight, 		float additionalHeight=0.f );

	Vec2			GetUVForPoint( const Vec2& point ) const;
	Vec2			GetPointForUV( const Vec2& normalizedLocalPosition ) const;

	void			TranslateByVector(const Vec2& translation);
};