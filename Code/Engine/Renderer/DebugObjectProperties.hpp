//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
enum eDebugRenderObject
{
	DEBUG_RENDER_POINT,
	DEBUG_RENDER_POINT3D,

	DEBUG_RENDER_LINE,
	DEBUG_RENDER_LINE3D,

	DEBUG_RENDER_QUAD,
	DEBUG_RENDER_TEX_QUAD,
	DEBUG_RENDER_WIRE_QUAD,

	DEBUG_RENDER_SPHERE,
	DEBUG_RENDER_WIRE_SPHERE,	//This has to be an icoSphere

	DEBUG_RENDER_BOX,
	DEBUG_RENDER_WIRE_BOX,

	DEBUG_RENDER_ARROW,
	DEBUG_RENDER_ARROW3D,

	DEBUG_RENDER_BASIS,
};

//------------------------------------------------------------------------------------------------------------------------------
class ObjectProperties
{
public:
	eDebugRenderObject m_renderObjectType;
	float m_durationSeconds         = 0.0f;  // show for a single frame
};

//------------------------------------------------------------------------------------------------------------------------------
//	2D Render Objects
//------------------------------------------------------------------------------------------------------------------------------
class Point2DObjectProperties : public ObjectProperties
{
public:
	explicit Point2DObjectProperties(eDebugRenderObject renderObject, const Vec2& screenPosition, float durationSeconds, float size);
	~Point2DObjectProperties();

public:
	Vec2 m_screenPosition				= Vec2::ZERO;
	float m_size						= DEFAULT_POINT_SIZE;
};

class Line2DObjectProperties : public ObjectProperties
{
public:
	explicit Line2DObjectProperties(eDebugRenderObject renderObject, const Vec2& startPos, const Vec2& endPos, float durationSeconds, float lineWidth);
	~Line2DObjectProperties();

public:
	Vec2 m_startPos						= Vec2::ZERO;
	Vec2 m_endPos						= Vec2::ZERO;
	float m_lineWidth					= DEFAULT_LINE_WIDTH;

};

//------------------------------------------------------------------------------------------------------------------------------
//	3D Render Objects
//------------------------------------------------------------------------------------------------------------------------------
class Point3DObjectProperties : public ObjectProperties
{
public:
	explicit Point3DObjectProperties(eDebugRenderObject renderObject);
	~Point3DObjectProperties();


};