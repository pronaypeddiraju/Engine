//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/DebugObjectProperties.hpp"

//------------------------------------------------------------------------------------------------------------------------------
//	2D Object Properties
//------------------------------------------------------------------------------------------------------------------------------
Point2DObjectProperties::Point2DObjectProperties( eDebugRenderObject renderObject, const Vec2& screenPosition, float durationSeconds, float size /*= DEFAULT_POINT_SIZE*/ )
{
	//Base properties
	m_durationSeconds = durationSeconds;
	m_startDuration = m_durationSeconds;
	m_renderObjectType = renderObject;

	//Point properties
	m_screenPosition = screenPosition;
	m_size = size;
}

Point2DObjectProperties::~Point2DObjectProperties()
{

}

Line2DObjectProperties::Line2DObjectProperties( eDebugRenderObject renderObject, const Vec2& startPos, const Vec2& endPos, float durationSeconds, float lineWidth )
{
	//Base properties
	m_durationSeconds = durationSeconds;
	m_startDuration = m_durationSeconds;
	m_renderObjectType = renderObject;

	//Line properties
	m_startPos = startPos;
	m_endPos = endPos;
	m_lineWidth = lineWidth;
}

Line2DObjectProperties::~Line2DObjectProperties()
{

}
