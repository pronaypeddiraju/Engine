//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec3.hpp"
#include <vector>

typedef unsigned int uint;

class SpriteDefenition;
class TextureView;

//------------------------------------------------------------------------------------------------------------------------------
class IsoSpriteDefenition
{
public:
	explicit IsoSpriteDefenition(SpriteDefenition* spriteDefenitions, const Vec3 directions[], uint numDefenitions);
	~IsoSpriteDefenition();

	SpriteDefenition* GetSpriteForLocalDirection(const Vec3& direction);

private:
	std::vector<SpriteDefenition*> m_sprites;
	//We have to determine which out of the 8 quadrants the entity is on
	std::vector<Vec3> m_directions;
};