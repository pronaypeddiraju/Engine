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

Quad2DProperties::Quad2DProperties( eDebugRenderObject renderObject, const AABB2& quad, float durationSeconds, float thickness, TextureView* texture )
{
	//Base properties
	m_durationSeconds = durationSeconds;
	m_startDuration = m_durationSeconds;
	m_renderObjectType = renderObject;

	//Quad Properties
	m_quad = quad;
	m_texture = texture;
	m_thickness = thickness;
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

Disc2DProperties::Disc2DProperties( eDebugRenderObject renderObject, const Disc2D& disc, float thickness, float durationSeconds /*= 0.f*/ )
{
	//Base properties
	m_durationSeconds = durationSeconds;
	m_startDuration = m_durationSeconds;
	m_renderObjectType = renderObject;

	//Disc properties
	m_disc = disc;
	m_thickness = thickness;
}

Disc2DProperties::~Disc2DProperties()
{

}

Line3DProperties::Line3DProperties( eDebugRenderObject renderObject, const Vec3& startPos, const Vec3& endPos, float durationSeconds /*= 0.f*/, float lineWidth /*= DEFAULT_LINE_WIDTH*/ )
{
	//Base properties
	m_durationSeconds = durationSeconds;
	m_startDuration = m_durationSeconds;
	m_renderObjectType = renderObject;

	//Line properties
	m_startPos = startPos;
	m_endPos = endPos;
	m_lineWidth = lineWidth;

	Vec3 disp = m_startPos - m_endPos;
	float length = disp.GetLength();
	Vec3 norm = disp.GetNormalized();
	Vec2 tangentXZ = Vec2(norm.x, norm.y).GetRotated90Degrees();
	Vec3 tangent = Vec3(tangentXZ.x, 0.f, tangentXZ.y).GetNormalized();
	Vec3 center = m_endPos + norm * length * 0.5f;

	Vec3 mins = Vec3(m_startPos.x, m_startPos.y - (tangent * lineWidth * 0.5f).y, m_startPos.x);
	Vec3 maxs = Vec3(m_endPos.x, m_endPos.y + (tangent * lineWidth * 0.5f).y, m_endPos.x);

	m_line = AABB2(mins, maxs);

	m_center = center;
	
}

Line3DProperties::~Line3DProperties()
{

}

SphereProperties::SphereProperties( eDebugRenderObject renderObject, const Vec3& center, float radius, float durationSeconds /*= 0.f*/, TextureView* texture /*= nullptr*/ )
{
	//Base properties
	m_durationSeconds = durationSeconds;
	m_startDuration = m_durationSeconds;
	m_renderObjectType = renderObject;

	//Sphere properties
	m_center = center;
	m_radius = radius;
	m_texture = texture;

	m_mesh = new CPUMesh();
	m_mesh->Clear();
	CPUMeshAddUVSphere( m_mesh, center, radius);  
}

SphereProperties::~SphereProperties()
{

}

BoxProperties::BoxProperties( eDebugRenderObject renderObject, const AABB3& box, float durationSeconds /*= 0.f*/, TextureView* texture /*= nullptr*/ )
{
	//Base properties
	m_durationSeconds = durationSeconds;
	m_startDuration = m_durationSeconds;
	m_renderObjectType = renderObject;

	//Box properties
	m_box = box;
	m_texture = texture;

	m_mesh = new CPUMesh();
	m_mesh->Clear();
	CPUMeshAddCube( m_mesh, box);
}

BoxProperties::~BoxProperties()
{

}
