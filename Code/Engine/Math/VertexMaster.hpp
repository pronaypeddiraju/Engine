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
	Vec3 m_position;							
	Vec3 m_normal;								
	Vec3 m_tangent;								
	Vec3 m_biTangent;							

	Rgba m_color = Rgba::WHITE;					
	Vec2 m_uv = Vec2::ZERO;						
}; 