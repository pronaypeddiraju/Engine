//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"

//------------------------------------------------------------------------------------------------------------------------------
enum eTextBoxMode
{
	TEXT_BOX_MODE_OVERLAP,
	TEXT_BOX_MODE_SHRINK
};

//------------------------------------------------------------------------------------------------------------------------------
enum eFontType
{
	FIXED_WIDTH,
	PROPORTIONAL,
	VARIABLE_WIDTH,	
	SIGNED_DISTANCE_FIELD
};

//------------------------------------------------------------------------------------------------------------------------------
struct GlyphData
{
	float m_aspectRatio = 1.0f;

	Vec2 m_texCoordMins = Vec2::ZERO;
	Vec2 m_texCoordMaxs = Vec2::ONE;

	float m_paddedWidthBeforeGlyph = 0.f;	//A value in TTF font
	float m_paddedWidthAtGlyph = 1.f;	//B value in TTF font
	float m_paddedWidthAfterGlyph = 0.f;	//C value in TTF font
};

//------------------------------------------------------------------------------------------------------------------------------
struct VariableFontLoader
{
	int				m_lineHeight = 0;
	IntVec2			m_sheetSize = IntVec2::ZERO;
	int				m_pages = 0;
	int				m_numChars = 0;
};

//------------------------------------------------------------------------------------------------------------------------------
class BitmapFont
{
	friend class RenderContext;

public:
	BitmapFont(std::string bitmapName, TextureView* bitmapTexture, eFontType fontType = FIXED_WIDTH, const IntVec2& sheetSplitSize = IntVec2(16,16));
	BitmapFont(std::string bitmapName, Image& bitmapImage, eFontType fontType = FIXED_WIDTH, const IntVec2& spriteSplitSize = IntVec2(16, 16), const std::string& XMLFilePath = "");
	
	//Initializer Functions
	void						InitializeProportionalFonts(Image& bitmapImage, const IntVec2& spriteSplitSize);
	void						GetLimitsForSpriteIndex(int index, int& limitX, int& limitY, const IntVec2& spriteSplitSize);
	void						MakeSpriteSheetfromImage(Image& bitmapImage, const IntVec2& spriteSplitSize);

	void						InitializeVariableWidthFonts(Image& bitmapImage, const IntVec2& spriteSplitSize, const std::string& XMLFilePath);

	float						GetGlyphAspect(int glyphCode);
	GlyphData&					GetGlyphData(int glyphCode);
	AABB2&						GetTextBoundingBox();
	float						GetNewCellAspect3D(const AABB2& box, float cellHeight, float cellAspect, const std::string& printText);
	float						GetNewCellAspect(const AABB2& box, float cellHeight, float cellAspect, const std::string& printText);
	TextureView*				GetTexture();

	//Rendering Functions
	void						AddVertsForText2D( std::vector<Vertex_PCU>& textVerts, const Vec2& textStartPosition, float cellHeight, std::string printText,
								const Rgba &tintColor = Rgba::WHITE, float cellAspect = 1.f, int maxGlyphsToDraw = 999999999);

	void						AddVertsForText3D( std::vector<Vertex_PCU>& textVerts, const Vec3& textStartPosition, float cellHeight, std::string printText,
								const Rgba &tintColor = Rgba::WHITE, float cellAspect = 1.f, int maxGlyphsToDraw = 999999999);

	void						AddVertsForTextInBox2D( std::vector<Vertex_PCU>& textVerts, const AABB2& box, float cellHeight,
								const std::string& printText, const Rgba& tintColor = Rgba::WHITE, float cellAspect = 1.f,
								const Vec2& alignment = Vec2::ALIGN_CENTERED, eTextBoxMode mode = TEXT_BOX_MODE_SHRINK, int maxGlyphsToDraw = 999999999 );

	void						AddVertsForTextInBox3D( std::vector<Vertex_PCU>& textVerts, const AABB2& box, float cellHeight,
								const std::string& printText, const Rgba& tintColor = Rgba::WHITE, float cellAspect = 1.f,
								const Vec2& alignment = Vec2::ALIGN_CENTERED, eTextBoxMode mode = TEXT_BOX_MODE_SHRINK, int maxGlyphsToDraw = 999999999 );								
private:

	eFontType					m_fontType = FIXED_WIDTH;
	std::string					m_bitmapName;
	SpriteSheet*				m_bitmapSpriteSheet;
	TextureView*				m_bitmapTexture;

	GlyphData					m_asciiGlyphData[256];	//For all 256 ascii characters
	float						m_baseAspect = 1.f;

	int							m_splitPadding = 1;	//The amount of padding we wont on our font image split. By default 1

	AABB2						m_boundingBox = AABB2(Vec2::ZERO, Vec2::ONE);

	int							m_maxSupportedCharacters = 256;

	VariableFontLoader			m_variableFontData;
};