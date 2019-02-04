#include "Engine/Math/Disc2D.hpp"

Disc2D::Disc2D(const Vec2& centre, float radius)
{
	m_discCentre = centre;
	m_radius = radius;
}

Disc2D::~Disc2D()
{

}

const Vec2& Disc2D::GetCentre() const
{
	return m_discCentre;
}

float Disc2D::GetRadius()
{
	return m_radius;
}

