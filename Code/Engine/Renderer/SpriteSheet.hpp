#pragma once
#include <vector>
#include "Engine/Renderer/SpriteDefenition.hpp"
#include "Engine/Math/IntVec2.hpp"

class Texture;
struct IntVec2;

class SpriteSheet
{
public: 
	
	explicit SpriteSheet(const Texture* texture, const IntVec2 spriteGridDefenition);
	
	//Accessors
	const SpriteDefenition& GetSpriteDef(int spriteIndex) const;
	const SpriteDefenition& GetSpriteDef( IntVec2 spriteCoords, int sheetWidth ) const;

protected:
	const Texture* m_spriteTexture = nullptr;
	std::vector<SpriteDefenition> m_spriteDefs;
};