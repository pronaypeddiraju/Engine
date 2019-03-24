//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
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

	DEBUG_RENDER_DISC,
	DEBUG_RENDER_RING,

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

	CPUMesh* m_mesh;
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
	explicit Quad2DProperties( eDebugRenderObject renderObject, const AABB2& quad, float durationSeconds = 0.f, float thickness = DEFAULT_WIRE_WIDTH_2D, TextureView* texture = nullptr );
	~Quad2DProperties();

public:
	TextureView* m_texture				= nullptr;
	float m_thickness					= DEFAULT_WIRE_WIDTH_2D;	//Only used when rendering as a wire box
	AABB2	m_quad;
};

// Disc
//------------------------------------------------------------------------------------------------------------------------------
class Disc2DProperties : public ObjectProperties
{
public:
	explicit Disc2DProperties( eDebugRenderObject renderObject, const Disc2D& disc, float thickness, float durationSeconds = 0.f);
	~Disc2DProperties();

public:
	Disc2D m_disc;
	float m_thickness;		//Only used when rendering it as a ring
};

//------------------------------------------------------------------------------------------------------------------------------
//	3D Render Objects
//------------------------------------------------------------------------------------------------------------------------------

// Point
//------------------------------------------------------------------------------------------------------------------------------
class Point3DProperties : public ObjectProperties
{
public:
	explicit Point3DProperties( eDebugRenderObject renderObject, const Vec3& position, float size, float durationSeconds = 0.f, TextureView* texture = nullptr);
	~Point3DProperties();

public:
	Vec3 m_position						= Vec3::ZERO;
	TextureView* m_texture				= nullptr;
	float m_size						= DEFAULT_POINT_SIZE_3D;
	
	AABB2 m_point;
};

// Line
//------------------------------------------------------------------------------------------------------------------------------
class Line3DProperties : public ObjectProperties
{
public:
	explicit Line3DProperties( eDebugRenderObject renderObject, const Vec3& startPos, const Vec3& endPos, float durationSeconds = 0.f, float lineWidth = DEFAULT_LINE_WIDTH);
	~Line3DProperties();

public:
	Vec3 m_startPos						= Vec3::ZERO;
	Vec3 m_endPos						= Vec3::ZERO;
	Vec3 m_center						= Vec3::ZERO;
	float m_lineWidth					= DEFAULT_LINE_WIDTH;

	AABB2 m_line;
};

// Sphere
//------------------------------------------------------------------------------------------------------------------------------
class SphereProperties : public ObjectProperties
{
public:
	explicit SphereProperties( eDebugRenderObject renderObject, const Vec3& center, float radius, float durationSeconds = 0.f, TextureView* texture = nullptr);
	~SphereProperties();

public:
	Vec3 m_center						= Vec3::ZERO;
	float m_radius						= 0.f;
	TextureView* m_texture				= nullptr;
};

// Box
//------------------------------------------------------------------------------------------------------------------------------
class BoxProperties : public ObjectProperties
{
public:
	explicit BoxProperties( eDebugRenderObject renderObject, const AABB3& box, float durationSeconds = 0.f, TextureView* texture = nullptr);
	~BoxProperties();

public:
	TextureView* m_texture				= nullptr;
	
	AABB3 m_box;
};