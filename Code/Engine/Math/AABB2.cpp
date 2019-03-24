//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/AABB2.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include <vector>
#include <string>

//------------------------------------------------------------------------------------------------------------------------------
AABB2::AABB2()
{

}

AABB2::AABB2( const Vec2 &minBounds, const Vec2 &maxBounds )
{
	m_minBounds = minBounds;
	m_maxBounds = maxBounds;

	m_center = Vec2(m_maxBounds.x - m_minBounds.x, m_maxBounds.y - m_minBounds.y);

	m_3Dmin = Vec3(minBounds.x, minBounds.y, 0.f);
	m_3Dmax = Vec3(maxBounds.x, maxBounds.y, 0.f);

	m_3Dcenter = Vec3(m_3Dmax - m_3Dmin);
}

AABB2::AABB2( const char* asText )
{
	SetFromText(asText);
}

AABB2::AABB2( const Vec3& min3D, const Vec3& max3D )
{
	m_3Dmin = min3D;
	m_3Dmax = max3D;

	//m_3Dcenter = Vec3(max3D - min3D);
}

const Vec2& AABB2::GetBoxCenter() const
{
	return m_center;
}

void AABB2::SetFromText( const char* asText )
{
	//Read the data, break using the delimiter and save each block to it's respective Vec2 component
	std::vector< std::string> splitStrings = SplitStringOnDelimiter(asText, ',');
	if(splitStrings.size() != 4)
	{
		ERROR_AND_DIE("ERROR: Data from AABB2 SetFromText did not recieve 4 string components");
	}
	else
	{
		m_minBounds = Vec2(static_cast<float>(atof(splitStrings[0].c_str())), static_cast<float>(atof(splitStrings[1].c_str())));
		m_maxBounds = Vec2(static_cast<float>(atof(splitStrings[2].c_str())), static_cast<float>(atof(splitStrings[3].c_str())));
	}
}

Vec3 AABB2::GetTopLeft() const
{
	return Vec3(m_3Dmin.x, m_3Dmax.y, m_3Dmin.z);
}

Vec3 AABB2::GetTopRight() const
{
	return Vec3(m_3Dmax.x, m_3Dmax.y, m_3Dmax.z);
}

Vec3 AABB2::GetBottomLeft() const
{
	return Vec3(m_3Dmin.x, m_3Dmin.y, m_3Dmin.z);
}

Vec3 AABB2::GetBottomRight() const
{
	return Vec3(m_3Dmax.x, m_3Dmin.y, m_3Dmax.z);
}

void AABB2::AlignWithinBox( const AABB2& box, const Vec2& alignment )
{
	float boxWidth = box.m_maxBounds.x - box.m_minBounds.x;
	float boxHeight = box.m_maxBounds.y - box.m_minBounds.y;

	float currentWidth = m_maxBounds.x - m_minBounds.x;
	float currentHeight = m_maxBounds.y - m_minBounds.y;

	
	//Ensure box is inside the align box
	m_minBounds.x = box.m_minBounds.x;
	m_maxBounds.x = m_minBounds.x + currentWidth;

	m_minBounds.y = box.m_minBounds.y;
	m_maxBounds.y = m_minBounds.y + currentHeight;

	//Alignmnet stuff
	float excessWidth = boxWidth - currentWidth;
	float excessHeight = boxHeight - currentHeight;

	m_minBounds.x += excessWidth * alignment.x;
	m_maxBounds.x = m_minBounds.x + currentWidth;

	m_minBounds.y += excessHeight * alignment.y;
	m_maxBounds.y = m_minBounds.y + currentHeight;

}

void AABB2::TranslateByVector( const Vec2& translation )
{
	m_minBounds += translation;
	m_maxBounds += translation;
}

AABB2::~AABB2()
{

}

