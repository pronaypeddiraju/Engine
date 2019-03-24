//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include <vector>

//------------------------------------------------------------------------------------------------------------------------------
struct AABB2;
struct AABB3;
class OBB2;

typedef unsigned int uint;

//------------------------------------------------------------------------------------------------------------------------------
// VertexMaster is a super-set of all vertex parameters we
// will want our CPUMesh to know about; 
// (Historically I would call CPUMesh a MeshBuilder, as it is used 
// for construction of a mesh before it is sent to the GPU.  Like an Image for a Texture
struct VertexMaster
{
	Vec3 position;          // A04
	Rgba color = Rgba::WHITE;             // A04
	Vec2 uv = Vec2::ZERO;                // A04;

	// vec3 normal;         // A05
	// vec4 tangent;        // A06
	
	// skin weights         // Summer
	// skin indices         // Summer
}; 

//------------------------------------------------------------------------------------------------------------------------------
class CPUMesh              // A04
{
public:
	CPUMesh();           // A04
	~CPUMesh();

	void Clear();        // A04; 

	// Modify the stamp;
	void SetColor( Rgba color );           // A04
	void SetUV( Vec2 uv );                 // A04

	// Stamp a vertex into the list - return the index; 
	uint AddVertex( VertexMaster const &m );     // A04
	uint AddVertex( Vec3 const &pos );           // A04

	// Adds a single triangle; 
	void AddIndexedTriangle( uint i0, uint i1, uint i2 );    // a04

	// adds two triangles (bl, tr, tl) and (bl, br, tr)
	void AddIndexedQuad( uint topLeft, uint topRight, uint bottomLeft, uint bottomRight );    // A04


																							  // Helpers
	uint GetVertexCount() const;                 // A04
	uint GetIndexCount() const;                  // A04

	inline bool UsesIndexBuffer() const          { return GetIndexCount() > 0; }
	inline uint GetElementCount() const          { return UsesIndexBuffer() ? GetIndexCount() : GetVertexCount(); }


public: 
	std::vector<VertexMaster>  m_vertices;       // A04
	std::vector<uint>          m_indices;        // A04

	VertexMaster m_stamp;                        // A04
};


//------------------------------------------------------------------------------------------------------------------------------
// I prefer keeping my construction functions as C functions
// as I like to pick and choose the ones I want instead of having them
// all live in CPUMesh.hpp
//------------------------------------------------------------------------------------------------------------------------------
void			CPUMeshAddQuad( CPUMesh *out, AABB2 quad, const Rgba& color = Rgba::WHITE); 
void			CPUMeshAddCube( CPUMesh *out, AABB3 box, const Rgba& color = Rgba::WHITE, bool clearMesh = true );                                                           // A04
void			CPUMeshAddUVSphere( CPUMesh *out, Vec3 center, float radius, const Rgba& color = Rgba::WHITE, uint wedges = 32, uint slices = 16 );							// A04
void			CPUMeshAddBox2D( CPUMesh *out, OBB2 const &obb, Rgba const &color = Rgba::WHITE);

