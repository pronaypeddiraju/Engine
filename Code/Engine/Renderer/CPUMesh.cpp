//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/ErrorWarningAssert.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Renderer/CPUMesh.hpp"

//------------------------------------------------------------------------------------------------------------------------------
CPUMesh::CPUMesh()
{
	Clear();
}

//------------------------------------------------------------------------------------------------------------------------------
void CPUMeshAddQuad( CPUMesh *out, const AABB2& quad, const Rgba& color)
{
	out->Clear(); 

	out->SetStampColor( color ); 
	out->SetNormal( Vec3::BACK ); 
	out->SetTangent( Vec3::RIGHT );
	out->SetBiTangent( Vec3::UP );

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

//------------------------------------------------------------------------------------------------------------------------------
void CPUMeshAddBox2D( CPUMesh *out, const OBB2& obb, Rgba const &color )
{
	out->Clear();

	out->SetStampColor( color ); 
	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3

	Vec2 corner = obb.GetTopLeft();

	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex(Vec3(corner.x, corner.y, 0.f)); 

	corner = obb.GetTopRight();
	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex(Vec3(corner.x, corner.y, 0.f)); 

	corner = obb.GetBottomLeft();
	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex(Vec3(corner.x, corner.y, 0.f)); 

	corner = obb.GetBottomRight();
	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex(Vec3(corner.x, corner.y, 0.f)); 

	out->AddIndexedTriangle( 0, 2, 1 ); 
	out->AddIndexedTriangle( 2, 3, 1 ); 
}

//------------------------------------------------------------------------------------------------------------------------------
void CPUMeshAddBoxFace(CPUMesh *out, const AABB2& quad)
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

//------------------------------------------------------------------------------------------------------------------------------
void CPUMeshAddCube( CPUMesh *out, const AABB3& box, const Rgba& color, bool clearMesh )
{
	if(clearMesh)
	{
		out->Clear();
	}

	out->SetStampColor( color ); 
	out->SetNormal( Vec3::BACK );
	out->SetTangent( Vec3::RIGHT );
	out->SetBiTangent( Vec3::UP );

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
	// 4 --- 5
	// |   / |
	// | /   |
	// 6 --- 7

	out->SetNormal( Vec3::FRONT );
	out->SetTangent( Vec3::LEFT );
	out->SetBiTangent( Vec3::UP );

	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex( box.m_backTopLeft); 
	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex( box.m_backTopRight ); 
	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex( box.m_backBottomLeft ); 
	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex( box.m_backBottomRight ); 

	out->AddIndexedTriangle( 4, 6, 5 ); 
	out->AddIndexedTriangle( 6, 7, 5 ); 

	//Add top face
	// 8 --- 9
	// |   / |
	// | /   |
	// 10 --- 11

	out->SetNormal( Vec3::UP );
	out->SetTangent( Vec3::RIGHT );
	out->SetBiTangent( Vec3::FRONT );

	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex( box.m_backTopRight); 
	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex( box.m_backTopLeft ); 
	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex( box.m_frontTopLeft ); 
	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex( box.m_frontTopRight ); 

	out->AddIndexedTriangle( 8, 10, 9 ); 
	out->AddIndexedTriangle( 10, 11, 9 );

	//Add bottom face
	// 12 --- 13
	// |   / |
	// | /   |
	// 14 --- 15

	out->SetNormal( Vec3::DOWN );
	out->SetTangent( Vec3::RIGHT );
	out->SetBiTangent( Vec3::BACK );

	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex( box.m_frontBottomLeft); 
	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex( box.m_frontBottomRight ); 
	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex( box.m_backBottomRight ); 
	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex( box.m_backBottomLeft ); 

	out->AddIndexedTriangle( 12, 14, 13 ); 
	out->AddIndexedTriangle( 14, 15, 13 );

	//Add left face
	// 16 --- 17
	// |   / |
	// | /   |
	// 18 --- 19

	out->SetNormal( Vec3::LEFT );
	out->SetTangent( Vec3::BACK );
	out->SetBiTangent( Vec3::UP );

	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex( box.m_backTopRight); 
	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex( box.m_frontTopLeft); 
	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex( box.m_backBottomRight ); 
	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex( box.m_frontBottomLeft ); 

	out->AddIndexedTriangle( 16, 18, 17 ); 
	out->AddIndexedTriangle( 18, 19, 17 );

	//Add right face
	// 20 --- 21
	// |   / |
	// | /   |
	// 22 --- 23

	out->SetNormal( Vec3::RIGHT );
	out->SetTangent( Vec3::FRONT );
	out->SetBiTangent( Vec3::UP );

	out->SetUV( Vec2(0.0f, 0.0f) ); 
	out->AddVertex( box.m_frontTopRight); 
	out->SetUV( Vec2(1.0f, 0.0f) ); 
	out->AddVertex( box.m_backTopLeft); 
	out->SetUV( Vec2(0.0f, 1.0f) ); 
	out->AddVertex( box.m_frontBottomRight ); 
	out->SetUV( Vec2(1.0f, 1.0f) ); 
	out->AddVertex( box.m_backBottomLeft ); 

	out->AddIndexedTriangle( 20, 22, 21 ); 
	out->AddIndexedTriangle( 22, 23, 21 );
}

//------------------------------------------------------------------------------------------------------------------------------
void CPUMeshAddUVSphere( CPUMesh *out, const Vec3& center, float radius, const Rgba& color, uint wedges /*= 32*/, uint slices /*= 16 */ )
{
	out->Clear();
	out->SetStampColor( color ); 

	int ustep = wedges + 1;
	int vstep = slices + 1;

	float phi;			//Angle along the j,k plane
	float theta;		//Angle along the i,k plane

	//Map out all the vertices
	for(int vIndex = 0; vIndex < vstep; vIndex++)
	{
		float v = static_cast<float>(vIndex) / static_cast<float>(slices);
		phi = RangeMapFloat(v, 0.f, 1.f, 90.f, 270.f);

		for(int uIndex = 0; uIndex < ustep; uIndex++)
		{
			float u = static_cast<float>(uIndex) / static_cast<float>(wedges);
			theta = u * 360.f;
			Vec3 position = GetSphericalToCartesian(radius, theta, phi) + center;

			//Get the normal to the vertex
			Vec3 norm = position - center;
			norm = norm.GetNormalized();

			//Get the tangent and bi tangent to the vertex
			Vec3 tangent = Vec3( -1.f * CosDegrees(phi) * SinDegrees(theta), 0.f, CosDegrees(phi) * CosDegrees(theta));
			Vec3 biTangent = GetCrossProduct(tangent, norm);

			out->SetUV(Vec2(u,v));
			out->SetNormal(norm);
			out->SetTangent(tangent);
			out->SetBiTangent(biTangent);
			out->AddVertex(position);
		}
	}

	//Map out all the Indices
	for(int y = 0; y < static_cast<int>(slices); y++)
	{
		for(int x = 0; x < static_cast<int>(wedges); x++)
		{
			uint TL = y * ustep + x;
			uint TR = TL + 1;
			uint BL = TL + ustep;
			uint BR = BL + 1;
			out->AddIndexedQuad(TL, TR, BL, BR);
		}
	}

}

//------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------
uint CPUMesh::GetIndexCount() const
{
	return static_cast<int>(m_indices.size());
}

//------------------------------------------------------------------------------------------------------------------------------
uint CPUMesh::GetVertexCount() const
{
	return static_cast<int>(m_vertices.size());
}

//------------------------------------------------------------------------------------------------------------------------------
void CPUMesh::Clear()
{
	m_vertices.clear();
	m_indices.clear();

	m_stamp.m_position = Vec3::ZERO;
	m_stamp.m_color = Rgba::WHITE;
	m_stamp.m_uv = Vec2::ZERO;
}

//------------------------------------------------------------------------------------------------------------------------------
CPUMesh::~CPUMesh()
{
	Clear();
}

//------------------------------------------------------------------------------------------------------------------------------
void CPUMesh::SetColor( const Rgba& color )
{
	int vertexCount = (int)m_vertices.size();

	for(int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
	{
		m_vertices[vertexIndex].m_color = color;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void CPUMesh::SetStampColor( const Rgba& color)
{
	m_stamp.m_color = color;
}

//------------------------------------------------------------------------------------------------------------------------------
void CPUMesh::SetUV( const Vec2& uv )
{
	m_stamp.m_uv = uv;
}

//------------------------------------------------------------------------------------------------------------------------------
uint CPUMesh::AddVertex( VertexMaster const &m )
{
	m_vertices.push_back(m);
	
	uint index = static_cast<uint>(m_vertices.size());
	index--;
	
	return index;
}

//------------------------------------------------------------------------------------------------------------------------------
uint CPUMesh::AddVertex( Vec3 const &pos )
{
	VertexMaster m = m_stamp;
	m.m_position = pos;

	m_vertices.push_back(m);

	uint index = static_cast<uint>(m_vertices.size());
	index--;

	return index;
}

//------------------------------------------------------------------------------------------------------------------------------
void CPUMesh::AddIndexedQuad( uint topLeft, uint topRight, uint bottomLeft, uint bottomRight )
{
	AddIndexedTriangle(bottomLeft, topRight, topLeft);
	AddIndexedTriangle(bottomLeft, bottomRight, topRight);
}

//------------------------------------------------------------------------------------------------------------------------------
void CPUMesh::SetLayout( const BufferLayout* layout )
{
	m_layout = layout;
}

//------------------------------------------------------------------------------------------------------------------------------
BufferLayout const* CPUMesh::GetLayout() const
{
	return m_layout;
}

//------------------------------------------------------------------------------------------------------------------------------
VertexMaster const* CPUMesh::GetVertices() const
{
	return &m_vertices[0];
}

//------------------------------------------------------------------------------------------------------------------------------
uint const* CPUMesh::GetIndices() const
{
	return &m_indices[0];
}

//------------------------------------------------------------------------------------------------------------------------------
void CPUMesh::SetNormal( const Vec3& norm )
{
	m_stamp.m_normal = norm;
}

//------------------------------------------------------------------------------------------------------------------------------
void CPUMesh::SetTangent( const Vec3& tangent )
{
	m_stamp.m_tangent = tangent;
}

//------------------------------------------------------------------------------------------------------------------------------
void CPUMesh::SetBiTangent( const Vec3& biTangent )
{
	m_stamp.m_biTangent = biTangent;
}
