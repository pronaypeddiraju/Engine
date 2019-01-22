#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Math/MathUtils.hpp"

void AddVertsForDisc2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& center, float radius, const Rgba& color, int numSides /*= 64 */ )
{
	float angleToAdd = 360.f / numSides;
	Vec3 baseVector = Vec3(radius, 0.f, 0.f);
	Vec3 positionVector;
	Rgba vertexColor = color;
	for (float i = 0.f; i < 360.f; i += angleToAdd)
	{
		positionVector = Vec3(center.x, center.y, 0.f);
		vertexArray.push_back(Vertex_PCU(positionVector, vertexColor, Vec2(0.f,0.f)));

		positionVector = baseVector.GetRotatedAboutZDegrees(i);
		positionVector += Vec3(center.x, center.y, 0.f);
		vertexArray.push_back(Vertex_PCU(positionVector, vertexColor, Vec2(0.f,0.f)));

		positionVector = baseVector.GetRotatedAboutZDegrees(i + angleToAdd);
		positionVector += Vec3(center.x, center.y, 0.f);		
		vertexArray.push_back(Vertex_PCU(positionVector, vertexColor, Vec2(0.f,0.f)));
	}
}

void AddVertsForLine2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& start, const Vec2& end, float thickness, const Rgba& color )
{
	Vec2 centerToBoundVector = end - start;
	centerToBoundVector.SetLength(thickness/2);
	Vec2 centreToBoundRotated = centerToBoundVector.GetRotated90Degrees();

	//Point 1 (End left)
	Vec3 lineCorner = Vec3(end.x, end.y, 0.f) + Vec3(centerToBoundVector.x, centerToBoundVector.y, 0.f) + Vec3(centreToBoundRotated.x, centreToBoundRotated.y, 0.f);
	vertexArray.push_back(Vertex_PCU(lineCorner, color, Vec2(0.f, 0.f)));

	//Point 2 (End Right)
	lineCorner = Vec3(end.x, end.y, 0.f) + Vec3(centerToBoundVector.x, centerToBoundVector.y, 0.f) - Vec3(centreToBoundRotated.x, centreToBoundRotated.y, 0.f);
	vertexArray.push_back(Vertex_PCU(lineCorner, color, Vec2(0.f, 0.f)));

	//Point 3 (Start Left)
	lineCorner = Vec3(start.x, start.y, 0.f) - Vec3(centerToBoundVector.x, centerToBoundVector.y, 0.f) + Vec3(centreToBoundRotated.x, centreToBoundRotated.y, 0.f);
	vertexArray.push_back(Vertex_PCU(lineCorner, color, Vec2(0.f, 0.f)));

	//We have our first triangle now

	//Point 4 (start left)
	lineCorner = Vec3(start.x, start.y, 0.f) - Vec3(centerToBoundVector.x, centerToBoundVector.y, 0.f) + Vec3(centreToBoundRotated.x, centreToBoundRotated.y, 0.f);
	vertexArray.push_back(Vertex_PCU(lineCorner, color, Vec2(0.f, 0.f)));

	//Point 5 (Start Right)
	lineCorner = Vec3(start.x, start.y, 0.f) - Vec3(centerToBoundVector.x, centerToBoundVector.y, 0.f) - Vec3(centreToBoundRotated.x, centreToBoundRotated.y, 0.f);
	vertexArray.push_back(Vertex_PCU(lineCorner, color, Vec2(0.f, 0.f)));

	//Point 6 (End right)
	lineCorner = Vec3(end.x, end.y, 0.f) + Vec3(centerToBoundVector.x, centerToBoundVector.y, 0.f) - Vec3(centreToBoundRotated.x, centreToBoundRotated.y, 0.f);
	vertexArray.push_back(Vertex_PCU(lineCorner, color, Vec2(0.f, 0.f)));
}

void AddVertsForRing2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& center, float radius, float thickness, const Rgba& color, int numSides /*= 64 */ )
{
	float angleToAdd = 360.f / numSides;
	float rMin = radius - thickness/2;
	float rMax = radius + thickness/2;
	
	Vec3 innerPositionVector;
	Vec3 outerPositionVector;
	Vec3 innerNextPosition;
	Vec3 outerNextPosition;

	Rgba vertexColor = color;
	for (float i = 0.f; i < 360.f; i += angleToAdd)
	{
		innerPositionVector.x = center.x + rMin * CosDegrees(i);
		innerPositionVector.y = center.y + rMin * SinDegrees(i);
		innerPositionVector.z = 0.f;
		outerPositionVector.x = center.x + rMax * CosDegrees(i);
		outerPositionVector.y = center.y + rMax * SinDegrees(i);
		outerPositionVector.z = 0.f;

		innerNextPosition.x = center.x + rMin * CosDegrees(i + angleToAdd);
		innerNextPosition.y = center.y + rMin * SinDegrees(i + angleToAdd);
		innerNextPosition.z = 0.f;
		outerNextPosition.x = center.x + rMax * CosDegrees(i + angleToAdd);
		outerNextPosition.y = center.y + rMax * SinDegrees(i + angleToAdd);
		outerNextPosition.z = 0.f;

		//Vertex set 1
		vertexArray.push_back(Vertex_PCU(innerPositionVector, vertexColor, Vec2(0.f,0.f)));
		vertexArray.push_back(Vertex_PCU(outerPositionVector, vertexColor, Vec2(0.f,0.f)));
		vertexArray.push_back(Vertex_PCU(outerNextPosition, vertexColor, Vec2(0.f,0.f)));
		
		//Vertex set 2
		vertexArray.push_back(Vertex_PCU(outerNextPosition, vertexColor, Vec2(0.f,0.f)));
		vertexArray.push_back(Vertex_PCU(innerNextPosition, vertexColor, Vec2(0.f,0.f)));
		vertexArray.push_back(Vertex_PCU(innerPositionVector, vertexColor, Vec2(0.f,0.f)));
	}
}

void AddVertsForAABB2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, const Rgba& color, const Vec2& uvAtMins /*= Vec2(0.f,0.f)*/, const Vec2& uvAtMaxs /*= Vec2(1.f,1.f) */ )
{
	Vec3 boxBottomLeft = Vec3(box.m_minBounds.x, box.m_minBounds.y, 0.f);
	Vec3 boxBottomRight = Vec3(box.m_maxBounds.x, box.m_minBounds.y, 0.f);
	Vec3 boxTopLeft = Vec3(box.m_minBounds.x, box.m_maxBounds.y, 0.f);
	Vec3 boxTopRight = Vec3(box.m_maxBounds.x, box.m_maxBounds.y, 0.f);

	vertexArray.push_back(Vertex_PCU(boxBottomLeft, color, uvAtMins));
	vertexArray.push_back(Vertex_PCU(boxBottomRight, color, Vec2(uvAtMaxs.x, uvAtMins.y)));
	vertexArray.push_back(Vertex_PCU(boxTopRight, color, Vec2(uvAtMaxs.x, uvAtMaxs.y)));

	vertexArray.push_back(Vertex_PCU(boxBottomLeft, color, uvAtMins));
	vertexArray.push_back(Vertex_PCU(boxTopLeft, color, Vec2(uvAtMins.x, uvAtMaxs.y)));
	vertexArray.push_back(Vertex_PCU(boxTopRight, color, Vec2(uvAtMaxs.x, uvAtMaxs.y)));


}

//Move a vertex
void TransformVertex2D( Vertex_PCU& vertex, float uniformScale, float rotationDegreesOnZ, const Vec2& translateXY )
{
	//For now 
	UNUSED (vertex);
	UNUSED (uniformScale);
	UNUSED (rotationDegreesOnZ);
	UNUSED (translateXY);
}

//Move an array of vertices
void TransformVertexArray2D( int numVerts, Vertex_PCU* vertices, float uniformScale, float rotationDegreesOnZ, const Vec2& translateXY )
{
	for (int i = 0; i < numVerts; i++)
	{
		//Orientation
		vertices[i].m_position = vertices[i].m_position.GetRotatedAboutZDegrees(rotationDegreesOnZ);

		//Translation
		vertices[i].m_position = vertices[i].m_position + Vec3(translateXY.x, translateXY.y, 0.f);

		//Scaling
		vertices[i].m_position.x *= uniformScale;
		vertices[i].m_position.y *= uniformScale;
	}
}