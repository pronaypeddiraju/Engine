//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/DebugObjectProperties.hpp"
#include "Engine/Renderer/TextureView.hpp"

//------------------------------------------------------------------------------------------------------------------------------
//	2D Object Properties
//------------------------------------------------------------------------------------------------------------------------------
Point2DProperties::Point2DProperties( eDebugRenderObject renderObject, const Vec2& screenPosition, float durationSeconds, float size /*= DEFAULT_POINT_SIZE*/ )
{
	//Base properties
	m_durationSeconds = durationSeconds;
	m_startDuration = m_durationSeconds;
	m_renderObjectType = renderObject;

	//Point properties
	m_screenPosition = screenPosition;
	m_size = size;
}

Point2DProperties::~Point2DProperties()
{

}

Line2DProperties::Line2DProperties( eDebugRenderObject renderObject, const Vec2& startPos, const Vec2& endPos, float durationSeconds, float lineWidth )
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

Line2DProperties::~Line2DProperties()
{

}

Quad2DProperties::Quad2DProperties( eDebugRenderObject renderObject, const AABB2& quad, float durationSeconds )
{
	//Base properties
	m_durationSeconds = durationSeconds;
	m_startDuration = m_durationSeconds;
	m_renderObjectType = renderObject;

	//Quad Properties
	m_quad = quad;
}

Quad2DProperties::~Quad2DProperties()
{

}

Point3DProperties::Point3DProperties( eDebugRenderObject renderObject, const Vec3& position, float size, float durationSeconds /*= 0.f*/ , TextureView* texture /*= nullptr*/ )
{
	//Base properties
	m_durationSeconds = durationSeconds;
	m_startDuration = m_durationSeconds;
	m_renderObjectType = renderObject;

	//Point properties
	m_texture = texture;
	m_position = position;
	m_size = size;
}

Point3DProperties::~Point3DProperties()
{

}
