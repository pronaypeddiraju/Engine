#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Rgba.hpp"

struct Vertex_PCU
{
public:
	//public variables
	Vec3 m_position;
	//Vec2 m_position;
	Vec2 m_uvTexCoords;
	Rgba m_color;

private:
	//private variables

public:
	//public methods
	Vertex_PCU() {};
	~Vertex_PCU() {};
	explicit Vertex_PCU(const Vec3& position, const Rgba& color, const Vec2& uvTexCoords);

private:
	//private methods
};
