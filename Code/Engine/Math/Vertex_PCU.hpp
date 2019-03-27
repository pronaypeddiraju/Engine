//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Rgba.hpp"

//------------------------------------------------------------------------------------------------------------------------------
struct VertexMaster;
struct BufferAttributeT;

typedef unsigned int uint;

//------------------------------------------------------------------------------------------------------------------------------
struct Vertex_PCU
{

public:

	Vec3 m_position = Vec3::ZERO;
	Vec2 m_uvTexCoords = Vec2::ZERO;
	Rgba m_color = Rgba::WHITE;

public:

	explicit Vertex_PCU(const VertexMaster& master);
	Vertex_PCU() {};
	~Vertex_PCU() {};
	explicit Vertex_PCU(const Vec3& position, const Rgba& color, const Vec2& uvTexCoords);

	static BufferAttributeT LAYOUT[]; 
	static void CopyFromMaster( void *buffer, VertexMaster const *src, uint count );
};
