//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>

//------------------------------------------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);

class Capsule2D;
class OBB2;
struct AABB2;
struct Rgba;
struct Vertex_PCU;

//------------------------------------------------------------------------------------------------------------------------------
//Vertex Utils
void			AddVertsForDisc2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& center, float radius, const Rgba& color, int numSides = 64 );
void			AddVertsForLine2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& start, const Vec2& end, float thickness, const Rgba& color );
void			AddVertsForRing2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& center, float radius, float thickness, const Rgba& color, int numSides = 64 );
void			AddVertsForAABB2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, const Rgba& color, const Vec2& uvAtMins = Vec2(0.f,1.f), const Vec2& uvAtMaxs = Vec2(1.f,0.f) );
void			AddVertsForOBB2D( std::vector<Vertex_PCU>& vertexArray, const OBB2& box, const Rgba& color, const Vec2& uvAtMins = Vec2(0.f,1.f), const Vec2& uvAtMaxs = Vec2(1.f,0.f) );
void			AddVertsForCapsule2D( std::vector<Vertex_PCU>& vertexArray, const OBB2& capsule, float radius, const Rgba& color);

void			AddVertsForBoundingBox( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, const Rgba& color, float thickness);
void			AddVertsForBoundingBox( std::vector<Vertex_PCU>& vertexArray, const OBB2& box, const Rgba& color, float thickness);

//Transform Utils
void			TransformVertex2D( Vertex_PCU& vertex, float uniformScale, float rotationDegreesOnZ, const Vec2& translateXY);
void			TransformVertexArray2D(int numVerts, Vertex_PCU* vertices, float uniformScale, float rotationDegreesOnZ, const Vec2& translateXY);
