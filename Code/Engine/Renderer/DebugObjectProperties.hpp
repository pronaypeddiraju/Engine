//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Rgba.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class TextureView;

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
	float m_startDuration			= 0.f;
	Rgba m_currentColor				= Rgba::WHITE;
};

//------------------------------------------------------------------------------------------------------------------------------
//	2D Render Objects
//------------------------------------------------------------------------------------------------------------------------------

// Point
//------------------------------------------------------------------------------------------------------------------------------
class Point2DProperties : public ObjectProperties
{
public:
	explicit Point2DProperties(eDebugRenderObject renderObject, const Vec2& screenPosition, float durationSeconds, float size);
	~Point2DProperties();

public:
	Vec2 m_screenPosition				= Vec2::ZERO;
	float m_size						= DEFAULT_POINT_SIZE;
};

// Line
//------------------------------------------------------------------------------------------------------------------------------
class Line2DProperties : public ObjectProperties
{
public:
	explicit Line2DProperties(eDebugRenderObject renderObject, const Vec2& startPos, const Vec2& endPos, float durationSeconds, float lineWidth);
	~Line2DProperties();

public:
	Vec2 m_startPos						= Vec2::ZERO;
	Vec2 m_endPos						= Vec2::ZERO;
	float m_lineWidth					= DEFAULT_LINE_WIDTH;

};

// Quad
//------------------------------------------------------------------------------------------------------------------------------
class Quad2DProperties : public ObjectProperties
{
public:
	explicit Quad2DProperties( eDebugRenderObject renderObject, const AABB2& quad, float durationSeconds = 0.f );
	~Quad2DProperties();

public:
	AABB2	m_quad;
};

//------------------------------------------------------------------------------------------------------------------------------
//	3D Render Objects
//------------------------------------------------------------------------------------------------------------------------------
class Point3DProperties : public ObjectProperties
{
public:
	explicit Point3DProperties(eDebugRenderObject renderObject, const Vec3& position, float size, float durationSeconds = 0.f, TextureView* texture = nullptr);
	~Point3DProperties();

public:
	Vec3 m_position						= Vec3::ZERO;
	TextureView* m_texture				= nullptr;
	float m_size						= DEFAULT_POINT_SIZE_3D;
};