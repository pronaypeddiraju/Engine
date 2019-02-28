//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"

//------------------------------------------------------------------------------------------------------------------------------
enum TextBoxMode
{
	TEXT_BOX_MODE_OVERLAP,
	TEXT_BOX_MODE_SHRINK
};

//------------------------------------------------------------------------------------------------------------------------------
class BitmapFont
{
	friend class RenderContext;

public:
	BitmapFont(std::string bitmapName, TextureView* bitmapTexture);
	
	float						GetGlyphAspect(int glyphCode);
	AABB2&						GetTextBoundingBox();

	void						AddVertsForText2D( std::vector<Vertex_PCU>& textVerts, const Vec2& textStartPosition, float cellHeight, std::string printText,
								const Rgba &tintColor = Rgba::WHITE, float cellAspect = 1.f, int maxGlyphsToDraw = 999999999);

	void						AddVertsForTextInBox2D( std::vector<Vertex_PCU>& textVerts, const AABB2& box, float cellHeight,
								const std::string& printText, const Rgba& tintColor = Rgba::WHITE, float cellAspect = 1.f,
								const Vec2& alignment = Vec2::ALIGN_CENTERED, TextBoxMode mode = TEXT_BOX_MODE_SHRINK, int maxGlyphsToDraw = 999999999 );

	
	float						GetNewCellAspect( const AABB2& box, float cellHeight, float cellAspect, const std::string& printText );
	TextureView*				GetTexture();
private:

	std::string					m_bitmapName;
	SpriteSheet*				m_bitmapSpriteSheet;
	TextureView*				m_bitmapTexture;
	AABB2						m_boundingBox = AABB2(Vec2::ZERO, Vec2::ONE);
};