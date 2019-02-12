#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/Texture.hpp"

SpriteSheet::SpriteSheet( const TextureView* texture, const IntVec2 spriteGridDefenition )
	: m_spriteTexture(texture)
{

	//Setup the size of the stride for UVs on u and v
	float uPerSpriteGrid = 1.f / static_cast<float>(spriteGridDefenition.x);
	float vPerSpriteGrid = 1.f / static_cast<float>(spriteGridDefenition.y);

	//Get the area of the grid
	int numSprites = spriteGridDefenition.x * spriteGridDefenition.y;
	const int& spritesPerRow = spriteGridDefenition.x;

	for(int spriteIndex = 0; spriteIndex < numSprites; spriteIndex++)
	{
		//Get the coordinates on the sprite grid
		int spriteGridX = spriteIndex % spritesPerRow;
		int spriteGridY = spriteIndex / spritesPerRow;

		//Make the UVs min and max values
		float uAtMinX = static_cast<float>(spriteGridX) * uPerSpriteGrid;
		float uAtMaxX = uAtMinX + uPerSpriteGrid;

		float vAtMaxV = 1.f - (static_cast<float>(spriteGridY) * vPerSpriteGrid);
		float vAtMinV = vAtMaxV - vPerSpriteGrid;

		Vec2 UV_U = Vec2(uAtMinX, vAtMinV);
		Vec2 UV_V = Vec2(uAtMaxX, vAtMaxV);

		m_spriteDefs.push_back(SpriteDefenition(UV_U, UV_V));
	}
}

const SpriteDefenition& SpriteSheet::GetSpriteDef( int spriteIndex ) const
{
	return m_spriteDefs[spriteIndex];
}

const SpriteDefenition& SpriteSheet::GetSpriteDef( IntVec2 spriteCoords, int sheetWidth ) const
{
	//Calculate Sprite index from coordinates
	int spriteIndex = spriteCoords.x + (spriteCoords.y * sheetWidth);
	return m_spriteDefs[spriteIndex];
}

