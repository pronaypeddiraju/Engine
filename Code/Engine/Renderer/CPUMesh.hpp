//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/VertexMaster.hpp"
#include "Engine/Renderer/BufferLayout.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include <vector>

//------------------------------------------------------------------------------------------------------------------------------
struct AABB2;
struct AABB3;
class OBB2;

typedef unsigned int uint;

//------------------------------------------------------------------------------------------------------------------------------
class CPUMesh              // A04
{
public:
	CPUMesh();           // A04
	~CPUMesh();

	void Clear();        // A04; 

	// Modify the stamp;
	void SetColor( const Rgba& color );           // A04
	void SetUV( const Vec2& uv );                 // A04
	void SetNormal( const Vec3& norm );
	void SetTangent( const Vec3& tangent );
	void SetBiTangent( const Vec3& biTangent );
	void SetStampColor( const Rgba& color );

	void SetLayout( const BufferLayout* layout ); // A06

	// templated helper - so I can do things like
	// CPUMesh mesh;  
	// mesh.SetBufferLayout<VertexPCU>(); 
	template <typename T>
	void SetLayout() 
	{
		SetLayout( BufferLayout::For<T>() ); 
	}

	BufferLayout const*			GetLayout() const;       
	VertexMaster const*			GetVertices() const;     
	uint const*					GetIndices() const;
	uint*						GetIndicesEditable();

	// Stamp a vertex into the list - return the index; 
	uint						AddVertex( const VertexMaster& m );     
	uint						AddVertex( const Vec3& pos );           
	
	void						AddIndex( uint index);
	// Adds a single triangle; 
	void						AddIndexedTriangle( uint i0, uint i1, uint i2 );
	// adds two triangles (bl, tr, tl) and (bl, br, tr)
	void						AddIndexedQuad( uint topLeft, uint topRight, uint bottomLeft, uint bottomRight );    
	


	// Helpers
	uint		GetVertexCount() const;                 
	uint		GetIndexCount() const;                  

	inline bool UsesIndexBuffer() const          { return GetIndexCount() > 0; }
	inline uint GetElementCount() const          { return UsesIndexBuffer() ? GetIndexCount() : GetVertexCount(); }

private:
	std::vector<VertexMaster>  m_vertices;       // A04
	std::vector<uint>          m_indices;        // A04

	VertexMaster m_stamp;                        // A04
	const BufferLayout* m_layout;                // A06
};


//------------------------------------------------------------------------------------------------------------------------------
// I prefer keeping my construction functions as C functions
// as I like to pick and choose the ones I want instead of having them
// all live in CPUMesh.hpp
//------------------------------------------------------------------------------------------------------------------------------
void			CPUMeshAddQuad( CPUMesh *out, const AABB2& quad, const Rgba& color = Rgba::WHITE, const Vec2& minUVs = Vec2::ZERO, const Vec2& maxUVs = Vec2::ONE);
void			CPUMeshAddCube( CPUMesh *out, const AABB3& box, const Rgba& color = Rgba::WHITE, bool clearMesh = true, int meshLastIndex = 0);                                                           // A04
void			CPUMeshAddUVSphere( CPUMesh *out, const Vec3& center, float radius, const Rgba& color = Rgba::WHITE, uint wedges = 32, uint slices = 16 );							// A04
void			CPUMeshAddUVCapsule(CPUMesh *out, const Vec3& start, const Vec3& end, float radius, const Rgba& color, uint wedges = 32, uint slices = 16);
void			CPUMeshAddBox2D( CPUMesh *out, const OBB2& obb, Rgba const &color = Rgba::WHITE);

