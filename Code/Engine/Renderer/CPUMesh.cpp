#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"
#include "Engine/Math/AABB3.hpp"

CPUMesh::CPUMesh()
{
	Clear();
}

//------------------------------------------------------------------------
void CPUMeshAddQuad( CPUMesh *out, AABB2 quad)
{
	out->Clear(); 

	out->SetColor( Rgba::WHITE ); 
	// out->SetNormal( vec3::BACK ); 

	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3
	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex( quad.GetTopLeft() ); 

	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex( quad.GetTopRight() ); 

	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex( quad.GetBottomLeft() ); 

	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex( quad.GetBottomRight() ); 

	out->AddIndexedTriangle( 0, 2, 1 ); 
	out->AddIndexedTriangle( 2, 3, 1 ); 
}

void CPUMeshAddBoxFace(CPUMesh *out, AABB2 quad)
{
	out->SetColor( Rgba::WHITE ); 
	// out->SetNormal( vec3::BACK ); 

	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3
	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex( quad.GetTopLeft() ); 

	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex( quad.GetTopRight() ); 

	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex( quad.GetBottomLeft() ); 

	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex( quad.GetBottomRight() ); 

	out->AddIndexedTriangle( 0, 2, 1 ); 
	out->AddIndexedTriangle( 2, 3, 1 ); 
}

void CPUMeshAddCube( CPUMesh *out, AABB3 box, bool clearMesh )
{
	if(clearMesh)
	{
		out->Clear();
	}

	out->SetColor( Rgba::WHITE ); 

	AABB2 boxFace;

	//Add front face
	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3
	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex( box.m_frontTopLeft); 
	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex( box.m_frontTopRight ); 
	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex( box.m_frontBottomLeft ); 
	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex( box.m_frontBottomRight ); 

	out->AddIndexedTriangle( 0, 2, 1 ); 
	out->AddIndexedTriangle( 2, 3, 1 ); 

	//Add back face
	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3
	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex( box.m_backTopLeft); 
	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex( box.m_backTopRight ); 
	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex( box.m_backBottomLeft ); 
	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex( box.m_backBottomRight ); 

	out->AddIndexedTriangle( 3, 5, 4 ); 
	out->AddIndexedTriangle( 5, 6, 4 ); 

	//Add top face
	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3
	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex( box.m_backTopLeft); 
	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex( box.m_backTopRight ); 
	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex( box.m_frontTopLeft ); 
	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex( box.m_frontTopRight ); 

	out->AddIndexedTriangle( 6, 8, 7 ); 
	out->AddIndexedTriangle( 8, 9, 7 );

	//Add bottom face
	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3
	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex( box.m_backBottomLeft); 
	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex( box.m_backBottomRight ); 
	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex( box.m_frontBottomLeft ); 
	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex( box.m_frontBottomRight ); 

	out->AddIndexedTriangle( 9, 11, 10 ); 
	out->AddIndexedTriangle( 11, 12, 10 );

	//Add left face
	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3
	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex( box.m_backTopLeft); 
	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex( box.m_frontTopLeft); 
	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex( box.m_backBottomLeft ); 
	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex( box.m_frontBottomLeft ); 

	out->AddIndexedTriangle( 12, 14, 13 ); 
	out->AddIndexedTriangle( 14, 15, 13 );

	//Add right face
	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3
	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex( box.m_frontTopRight); 
	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex( box.m_backTopRight); 
	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex( box.m_frontBottomRight ); 
	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex( box.m_backBottomRight ); 

	out->AddIndexedTriangle( 15, 17, 16 ); 
	out->AddIndexedTriangle( 17, 18, 16 );
}

//------------------------------------------------------------------------
void CPUMesh::AddIndexedTriangle( uint i0, uint i1, uint i2 )
{
	ASSERT_RECOVERABLE( i0 < m_vertices.size() , "Index is greater than the number of vertices");
	ASSERT_RECOVERABLE( i1 < m_vertices.size() , "Index is greater than the number of vertices");
	ASSERT_RECOVERABLE( i2 < m_vertices.size() , "Index is greater than the number of vertices");
	//Push it into the vertex vector 

	m_indices.push_back(i0);
	m_indices.push_back(i1);
	m_indices.push_back(i2);
}

uint CPUMesh::GetIndexCount() const
{
	return static_cast<int>(m_indices.size());
}

uint CPUMesh::GetVertexCount() const
{
	return static_cast<int>(m_vertices.size());
}

void CPUMesh::Clear()
{
	m_vertices.empty();
	m_indices.empty();

	m_stamp.position = Vec3::ZERO;
	m_stamp.color = Rgba::WHITE;
	m_stamp.uv = Vec2::ZERO;
}

CPUMesh::~CPUMesh()
{
	Clear();
}

void CPUMesh::SetColor( Rgba color )
{
	m_stamp.color = color;
}

void CPUMesh::SetUV( Vec2 uv )
{
	m_stamp.uv = uv;
}

uint CPUMesh::AddVertex( VertexMaster const &m )
{
	m_vertices.push_back(m);
	
	uint index = static_cast<uint>(m_vertices.size());
	index--;
	
	return index;
}

uint CPUMesh::AddVertex( Vec3 const &pos )
{
	VertexMaster m = m_stamp;
	m.position = pos;

	m_vertices.push_back(m);

	uint index = static_cast<uint>(m_vertices.size());
	index--;

	return index;
}

void CPUMesh::AddIndexedQuad( uint topLeft, uint topRight, uint bottomLeft, uint bottomRight )
{
	AddIndexedTriangle(bottomLeft, topRight, topLeft);
	AddIndexedTriangle(bottomLeft, bottomRight, topRight);
}
