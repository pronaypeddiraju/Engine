//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/StringUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

//------------------------------------------------------------------------------------------------------------------------------
BitmapFont::BitmapFont( std::string bitmapName, TextureView* bitmapTexture )
	: m_bitmapName (bitmapName),
	m_bitmapTexture (bitmapTexture),
	m_bitmapSpriteSheet (new SpriteSheet(bitmapTexture, IntVec2(16,16)))
{

}

//------------------------------------------------------------------------------------------------------------------------------
float BitmapFont::GetGlyphAspect( int glyphCode )
{
	//For now we always assume the aspect to be 1;
	UNUSED(glyphCode);
	return 1.0f;
}

//------------------------------------------------------------------------------------------------------------------------------
AABB2& BitmapFont::GetTextBoundingBox()
{
	return m_boundingBox;
}

//------------------------------------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForText2D( std::vector<Vertex_PCU>& textVerts, const Vec2& textStartPosition, float cellHeight, std::string printText, const Rgba &color, float cellAspect, int maxGlyphsToDraw)
{
	//For now unused
	UNUSED(maxGlyphsToDraw);

	int numChars = static_cast<int>(printText.length());
	cellHeight = cellHeight * cellAspect;
	float textWidth = cellHeight * GetGlyphAspect(0);
	Vec2 minBounds = textStartPosition;
	m_boundingBox.m_minBounds = textStartPosition;
	for(int charIndex = 0; charIndex < numChars; charIndex++)
	{
		//Make your box
		Vec2 maxBounds = minBounds + Vec2(textWidth, cellHeight);
		m_boundingBox.m_maxBounds = maxBounds;
		AABB2 box = AABB2(minBounds, maxBounds);

		//Get the UVs
		SpriteDefenition sd = m_bitmapSpriteSheet->GetSpriteDef(printText[charIndex]);
		Vec2 minUVs = Vec2::ZERO;
		Vec2 maxUVs = Vec2::ZERO;
		sd.GetUVs(minUVs, maxUVs);

		//Now add the vertices for each char
		AddVertsForAABB2D(textVerts, box, color, minUVs, maxUVs);

		//Move the minBounds on x
		minBounds.x += textWidth;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForText3D( std::vector<Vertex_PCU>& textVerts, const Vec3& textStartPosition, float cellHeight, std::string printText, const Rgba &tintColor /*= Rgba::WHITE*/, float cellAspect /*= 1.f*/, int maxGlyphsToDraw /*= 999999999*/ )
{
	//For now unused
	UNUSED(maxGlyphsToDraw);

	int numChars = static_cast<int>(printText.length());
	cellHeight = cellHeight * cellAspect;
	float textWidth = cellHeight * GetGlyphAspect(0);
	Vec3 minBounds = textStartPosition;
	m_boundingBox.m_3Dmin = textStartPosition;

	for(int charIndex = 0; charIndex < numChars; charIndex++)
	{
		//Make your box
		Vec3 maxBounds = minBounds + Vec3(textWidth, cellHeight, 0.f);
		m_boundingBox.m_3Dmax = maxBounds;
		AABB2 box = AABB2(minBounds, maxBounds);

		//Get the UVs
		SpriteDefenition sd = m_bitmapSpriteSheet->GetSpriteDef(printText[charIndex]);
		Vec2 minUVs = Vec2::ZERO;
		Vec2 maxUVs = Vec2::ZERO;
		sd.GetUVs(minUVs, maxUVs);

		//Now add the vertices for each char
		AddVertsForAABB3D(textVerts, box, tintColor, minUVs, maxUVs);

		//Move the minBounds on x
		minBounds.x += textWidth;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForTextInBox2D( std::vector<Vertex_PCU>& textVerts, const AABB2& box, float cellHeight, const std::string& printText, const Rgba& tintColor /*= Rgba::WHITE*/, float cellAspect /*= 1.f*/, const Vec2& alignment /*= Vec2::ALIGN_CENTERED*/, TextBoxMode textBoxMode /*= TEXT_BOX_MODE_SHRINK*/, int maxGlyphsToDraw /*= 999999999 */ )
{
	UNUSED(maxGlyphsToDraw);

	//Check what the textBoxMode is and set the cellAspect if needs to be shrunk
	switch( textBoxMode )
	{
	case TEXT_BOX_MODE_OVERLAP:
	cellAspect = cellHeight;
	break;
	case TEXT_BOX_MODE_SHRINK:
	//Shrink based on the total width
	cellAspect = GetNewCellAspect(box, cellHeight, cellAspect, printText);
	break;
	default:
	break;
	}
		
	float correctedHeight = cellHeight * cellAspect;
	m_boundingBox.m_minBounds = box.m_minBounds;
	m_boundingBox.m_maxBounds = m_boundingBox.m_minBounds + Vec2(static_cast<float>(printText.length()) * correctedHeight * GetGlyphAspect(0), correctedHeight);

	//Align the bounding box within the box specified
	m_boundingBox.AlignWithinBox(box, alignment);

	//Add vertices for the text after you align the box
	AddVertsForText2D(textVerts, m_boundingBox.m_minBounds, cellHeight, printText, tintColor, cellAspect, maxGlyphsToDraw);
}

//------------------------------------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForTextInBox3D( std::vector<Vertex_PCU>& textVerts, const AABB2& box, float cellHeight, const std::string& printText, const Rgba& tintColor /*= Rgba::WHITE*/, float cellAspect /*= 1.f*/, const Vec2& alignment /*= Vec2::ALIGN_CENTERED*/, TextBoxMode mode /*= TEXT_BOX_MODE_SHRINK*/, int maxGlyphsToDraw /*= 999999999 */ )
{
	UNUSED(maxGlyphsToDraw);

	//Check what the textBoxMode is and set the cellAspect if needs to be shrunk
	switch( mode )
	{
	case TEXT_BOX_MODE_OVERLAP:
	cellAspect = cellHeight;
	break;
	case TEXT_BOX_MODE_SHRINK:
	//Shrink based on the total width
	cellAspect = GetNewCellAspect3D(box, cellHeight, cellAspect, printText);
	break;
	default:
	break;
	}

	float correctedHeight = cellHeight * cellAspect;
	m_boundingBox.m_3Dmin = box.m_3Dmin;
	m_boundingBox.m_3Dmax = m_boundingBox.m_3Dmin + Vec3(static_cast<float>(printText.length()) * correctedHeight * GetGlyphAspect(0), correctedHeight, 0.f);

	//Align the bounding box within the box specified
	m_boundingBox.AlignWithinBox(box, alignment);

	//Add vertices for the text after you align the box
	AddVertsForText3D(textVerts, m_boundingBox.m_3Dmin, cellHeight, printText, tintColor, cellAspect, maxGlyphsToDraw);
}

//------------------------------------------------------------------------------------------------------------------------------
float BitmapFont::GetNewCellAspect3D(const AABB2& box, float cellHeight, float cellAspect, const std::string& printText)
{
	float boxWidth = box.m_3Dmax.x - box.m_3Dmin.x;
	float boxHeight = box.m_3Dmax.y - box.m_3Dmin.y;
	float cellWidth = cellHeight * cellAspect;

	//For multi line support
	//std::vector<std::string> stringVector;
	//stringVector = SplitStringOnDelimiter(printText, '\n');

	int numChars = static_cast<int>(printText.length());
	float textWidth = cellWidth * numChars;
	//See if the text is getting out of the box
	if(cellHeight > boxHeight && textWidth > boxWidth)
	{
		//Fat a f!
		float heightAspect = boxHeight / cellHeight;
		float widthAspect = boxWidth / textWidth;

		if(heightAspect < widthAspect)
		{
			cellAspect = boxHeight / cellHeight;
			return cellAspect;
		}
		else
		{
			cellAspect = boxWidth / textWidth;
			return cellAspect;
		}
	}

	if(textWidth > boxWidth)
	{
		//Too wide bro
		cellAspect = boxWidth / textWidth;
		return cellAspect;
	}

	if(cellHeight > boxHeight)
	{
		//Too tall bro
		cellAspect = boxHeight / cellHeight;
		return cellAspect;
	}

	return cellAspect;
}

//------------------------------------------------------------------------------------------------------------------------------
float BitmapFont::GetNewCellAspect(const AABB2& box, float cellHeight, float cellAspect, const std::string& printText)
{
	float boxWidth = box.m_maxBounds.x - box.m_minBounds.x;
	float boxHeight = box.m_maxBounds.y - box.m_minBounds.y;
	float cellWidth = cellHeight * cellAspect;

	//For multi line support
	//std::vector<std::string> stringVector;
	//stringVector = SplitStringOnDelimiter(printText, '\n');

	int numChars = static_cast<int>(printText.length());
	float textWidth = cellWidth * numChars;
	//See if the text is getting out of the box
	if(cellHeight > boxHeight && textWidth > boxWidth)
	{
		//Fat a f!
		float heightAspect = boxHeight / cellHeight;
		float widthAspect = boxWidth / textWidth;

		if(heightAspect < widthAspect)
		{
			cellAspect = boxHeight / cellHeight;
			return cellAspect;
		}
		else
		{
			cellAspect = boxWidth / textWidth;
			return cellAspect;
		}
	}

	if(textWidth > boxWidth)
	{
		//Too wide bro
		cellAspect = boxWidth / textWidth;
		return cellAspect;
	}
	
	if(cellHeight > boxHeight)
	{
		//Too tall bro
		cellAspect = boxHeight / cellHeight;
		return cellAspect;
	}

	return cellAspect;
}

//------------------------------------------------------------------------------------------------------------------------------
TextureView* BitmapFont::GetTexture()
{
	return m_bitmapTexture;
}

