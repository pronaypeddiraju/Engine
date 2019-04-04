//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Rgba.hpp"

//------------------------------------------------------------------------------------------------------------------------------
// VertexMaster is a super-set of all vertex parameters we
// will want our CPUMesh to know about; 
// (Historically I would call CPUMesh a MeshBuilder, as it is used 
// for construction of a mesh before it is sent to the GPU.  Like an Image for a Texture
struct VertexMaster
{
	Vec3 m_position;							// A04
	Vec3 m_normal;								// A05
	Vec3 m_tangent;								// A07
	Vec3 m_biTangent;							// A07

	Rgba m_color = Rgba::WHITE;					// A04
	Vec2 m_uv = Vec2::ZERO;						// A04;


	// skin weights         // Summer
	// skin indices         // Summer
}; 