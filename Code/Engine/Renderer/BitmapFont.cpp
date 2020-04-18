//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/StringUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/XMLUtils/XMLUtils.hpp"

//------------------------------------------------------------------------------------------------------------------------------
BitmapFont::BitmapFont( std::string bitmapName, TextureView* bitmapTexture, eFontType fontType, const IntVec2& sheetSplitSize )
	: m_bitmapName (bitmapName),
	m_fontType (fontType),
	m_bitmapTexture (bitmapTexture),
	m_bitmapSpriteSheet (new SpriteSheet(bitmapTexture, sheetSplitSize))
{
}

//------------------------------------------------------------------------------------------------------------------------------
BitmapFont::BitmapFont(std::string bitmapName, Image& bitmapImage, eFontType fontType /*= FIXED_WIDTH*/, const IntVec2& spriteSplitSize /*= IntVec2(16, 16)*/, const std::string& XMLFilePath /*= ""*/)
	: m_bitmapName (bitmapName),
	m_fontType (fontType)
{
	switch (fontType)
	{
	case FIXED_WIDTH:
		MakeSpriteSheetfromImage(bitmapImage, spriteSplitSize);
		break;
	case PROPORTIONAL:
		InitializeProportionalFonts(bitmapImage, spriteSplitSize);
		break;
	case VARIABLE_WIDTH:	
		InitializeVariableWidthFonts(bitmapImage, spriteSplitSize, XMLFilePath);
		//MakeSpriteSheetfromImage(bitmapImage, spriteSplitSize);
		break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BitmapFont::InitializeProportionalFonts(Image& bitmapImage, const IntVec2& spriteSplitSize)
{
	//Here we need to check what pixels in the file are valid and use those to create our proportional fonts
	IntVec2 imageDims = bitmapImage.GetImageDimensions();
	Rgba texelColor;

	int baseHeight = 0;
	float aspectRatio = 0.f;

	int maxHeightGlyph = 0;
	int maxWidthGlyph = 0;

	int limitX = 0;
	int limitY = 0;

	bool row1Eleminate = false;

	std::vector<SpriteDefenition> spriteDefs;

	for (int spriteIndex = 0; spriteIndex < spriteSplitSize.x * spriteSplitSize.y; spriteIndex++)
	{
		GetLimitsForSpriteIndex(spriteIndex, limitX, limitY, spriteSplitSize);

		//For each split, we want to know what is the maxWidth and maxHeight to calculate an aspect ratio
		for (int xIndex = limitX; xIndex < spriteSplitSize.x + limitX; xIndex++)
		{
			for (int yIndex = limitY; yIndex < spriteSplitSize.y + limitY; yIndex++)
			{
				//We want to identify the colors for the font and get the correct aspect ratio
				texelColor = bitmapImage.GetTexelColor(IntVec2(xIndex, yIndex));
				if (texelColor != Rgba::WHITE)
				{
					texelColor = Rgba(0.f, 0.f, 0.f, 0.f);
					texelColor.a = 0.f;

					bitmapImage.SetTexelColor(IntVec2(xIndex, yIndex), texelColor);
				}
				else
				{
					if (yIndex - limitY > maxHeightGlyph)
					{
						maxHeightGlyph = yIndex - limitY;
					}

					if (xIndex - limitX > maxWidthGlyph)
					{
						maxWidthGlyph = xIndex - limitX;
					}
				}
			}

			//Eleminate the first row if we can
			for (int row1Y = limitY; row1Y < maxHeightGlyph + limitY; row1Y++)
			{
				if(xIndex > maxWidthGlyph)
					continue;

				//If we find no white pixels here we can eliminate
				texelColor = bitmapImage.GetTexelColor(IntVec2(xIndex, row1Y));
				if (texelColor != Rgba::WHITE)
				{
					row1Eleminate = true;
				}
				else
				{
					row1Eleminate = false;
				}
			}
		}

		if (spriteIndex == 0)
		{
			//This is the sprite to use for setting our base glyph height
			baseHeight = maxHeightGlyph;
		}

		if (maxWidthGlyph != 0 && maxHeightGlyph != 0)
		{
			maxWidthGlyph += m_splitPadding;
			maxHeightGlyph += m_splitPadding;

			if (maxHeightGlyph < baseHeight)
				maxHeightGlyph = baseHeight;

			aspectRatio = (float)(maxWidthGlyph) / (float)(maxHeightGlyph);	//Add 1 extra pixel of padding on width and height
			m_asciiGlyphData[spriteIndex].m_aspectRatio = aspectRatio;
		}

		//Set the UVs
		Vec2 mins;
		Vec2 maxs;

		mins.x = ((float)limitX / (float)imageDims.x);
		maxs.y = ((float)limitY / (float)imageDims.y);

		maxs.x = ((float)(limitX + maxWidthGlyph + m_splitPadding) / (float)imageDims.x);
		mins.y = ((float)(limitY + maxHeightGlyph) / (float)imageDims.y);

		spriteDefs.emplace_back(nullptr, mins , maxs);

		if (row1Eleminate)
		{
			aspectRatio = (float)(maxWidthGlyph - 1) / (float)(maxHeightGlyph);
			m_asciiGlyphData[spriteIndex].m_aspectRatio = aspectRatio;

			mins.x = ((float)(limitX + 1)/ (float)imageDims.x);
			spriteDefs[spriteDefs.size() - 1].SetUVs(mins, maxs);

			row1Eleminate = false;
		}

		maxHeightGlyph = 0;
		maxWidthGlyph = 0;
	}

	m_bitmapSpriteSheet = new SpriteSheet();
	m_bitmapSpriteSheet->SetSpriteDefs(spriteDefs);
	
	Texture2D* texture = Texture2D::CreateTextureFromImage(g_renderContext, bitmapImage);
	m_bitmapTexture = texture->CreateTextureView();

	delete texture;

	m_bitmapSpriteSheet->SetSpriteTextureView(m_bitmapTexture);
}

//------------------------------------------------------------------------------------------------------------------------------
void BitmapFont::GetLimitsForSpriteIndex(int index, int& limitX, int& limitY, const IntVec2& spriteSplitSize)
{
	if (index == 0)
	{
		limitX = 0;
		limitY = 0;
	}
	else
	{
		limitY = (index / spriteSplitSize.y) * spriteSplitSize.y;
		limitX = (index % spriteSplitSize.y) * spriteSplitSize.x;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void BitmapFont::MakeSpriteSheetfromImage(Image& bitmapImage, const IntVec2& spriteSplitSize)
{
	//Make the sprite sheet here
	Texture2D* texture = Texture2D::CreateTextureFromImage(g_renderContext, bitmapImage);
	m_bitmapTexture = texture->CreateTextureView();

	m_bitmapSpriteSheet = new SpriteSheet(m_bitmapTexture, spriteSplitSize);
}

//------------------------------------------------------------------------------------------------------------------------------
void BitmapFont::InitializeVariableWidthFonts(Image& bitmapImage, const IntVec2& spriteSplitSize, const std::string& XMLFilePath)
{
	UNUSED(spriteSplitSize);

	//Load XML information
	tinyxml2::XMLDocument fontDoc;
	fontDoc.LoadFile(XMLFilePath.c_str());

	if (fontDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{

		ERROR_AND_DIE(">> Error loading Font XML file ");
		return;
	}
	else
	{
		//We successfully loaded the file
		XMLElement* root = fontDoc.RootElement();
		XMLElement* node = root->FirstChildElement("common");

		//Get the size of the font sheet
		m_variableFontData.m_lineHeight = ParseXmlAttribute(*node, "lineHeight", m_variableFontData.m_lineHeight);
		m_variableFontData.m_sheetSize.x = ParseXmlAttribute(*node, "scaleW", m_variableFontData.m_sheetSize.x);
		m_variableFontData.m_sheetSize.y = ParseXmlAttribute(*node, "scaleH", m_variableFontData.m_sheetSize.y);

		m_variableFontData.m_pages = ParseXmlAttribute(*node, "pages", m_variableFontData.m_pages);
		ASSERT_RECOVERABLE(m_variableFontData.m_pages == 1, "More than 1 pages of fonts used with Variable Width font file");

		node = root->FirstChildElement("chars");
		m_variableFontData.m_numChars = ParseXmlAttribute(*node, "count", m_variableFontData.m_numChars);

		std::vector<SpriteDefenition> spriteDefs(m_maxSupportedCharacters, SpriteDefenition(nullptr, Vec2::ZERO, Vec2::ONE));

		node = node->FirstChildElement("char");
		for (int charIndex = 0; charIndex < m_variableFontData.m_numChars; charIndex++)
		{
			int charID = ParseXmlAttribute(*node, "id", 0);
			
			float xPosition = ParseXmlAttribute(*node, "x", 0.f);
			float yPosition = ParseXmlAttribute(*node, "y", 0.f);
			int width = ParseXmlAttribute(*node, "width", 0);
			int height = ParseXmlAttribute(*node, "height", 0);

			float xOffset = ParseXmlAttribute(*node, "xoffset", 0.f);
			float yOffset = ParseXmlAttribute(*node, "yoffset", 0.f);
			float xAdvance = ParseXmlAttribute(*node, "xadvance", 0.f);

			m_asciiGlyphData[charID].m_paddedWidthBeforeGlyph = xOffset / (float)m_variableFontData.m_lineHeight;
			m_asciiGlyphData[charID].m_paddedWidthAtGlyph = xAdvance / (float)m_variableFontData.m_lineHeight;
			m_asciiGlyphData[charID].m_paddedWidthAfterGlyph = (xAdvance - xOffset - width) / (float)m_variableFontData.m_lineHeight;

			m_asciiGlyphData[charID].m_aspectRatio = (float)width / (float)m_variableFontData.m_lineHeight;
			m_asciiGlyphData[charID].m_texCoordMins = Vec2(xPosition, yPosition + (float)m_variableFontData.m_lineHeight) / (float)m_variableFontData.m_sheetSize.x;
			m_asciiGlyphData[charID].m_texCoordMaxs = m_asciiGlyphData[charID].m_texCoordMins + Vec2((float)width, -(float)m_variableFontData.m_lineHeight) / (float)m_variableFontData.m_sheetSize.x;

			spriteDefs[charID].SetUVs(m_asciiGlyphData[charID].m_texCoordMins, m_asciiGlyphData[charID].m_texCoordMaxs);

			//Handle special case for Space character
			if (charID == 32)
			{
				m_asciiGlyphData[charID].m_aspectRatio = xAdvance / (float)m_variableFontData.m_lineHeight;

				spriteDefs[charID].SetUVs(m_asciiGlyphData[charID].m_texCoordMins, m_asciiGlyphData[charID].m_texCoordMaxs);
			}

			node = node->NextSiblingElement();
		}

		Rgba texelColor;
		//Remove all alpha on font sheet
		for (int i = 0; i < m_variableFontData.m_sheetSize.x; i++)
		{
			for (int j = 0; j < m_variableFontData.m_sheetSize.y; j++)
			{
				texelColor = bitmapImage.GetTexelColor(IntVec2(i, j));
				if (texelColor != Rgba::WHITE)
				{
					texelColor = Rgba(0.f, 0.f, 0.f, 0.f);
					texelColor.a = 0.f;

					bitmapImage.SetTexelColor(IntVec2(i, j), texelColor);
				}
			}
		}

		m_bitmapSpriteSheet = new SpriteSheet();
		m_bitmapSpriteSheet->SetSpriteDefs(spriteDefs);

		Texture2D* texture = Texture2D::CreateTextureFromImage(g_renderContext, bitmapImage);
		m_bitmapTexture = texture->CreateTextureView();

		delete texture;

		m_bitmapSpriteSheet->SetSpriteTextureView(m_bitmapTexture);
	}

}

//------------------------------------------------------------------------------------------------------------------------------
float BitmapFont::GetGlyphAspect( int glyphCode )
{
	return m_asciiGlyphData[glyphCode].m_aspectRatio;
}

//------------------------------------------------------------------------------------------------------------------------------
GlyphData& BitmapFont::GetGlyphData(int glyphCode)
{
	return m_asciiGlyphData[glyphCode];
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
	float textWidth;
	float glyphAspect;
	Vec2 minBounds = textStartPosition;
	m_boundingBox.m_minBounds = textStartPosition;
	for(int charIndex = 0; charIndex < numChars; charIndex++)
	{
		//Get the correct glyph aspect for each box
		glyphAspect = cellAspect * GetGlyphAspect((int)printText[charIndex]);
		textWidth = glyphAspect * cellHeight ;

		//Make your box (Now use the ABC values from GlyphData)
		GlyphData data = GetGlyphData((int)printText[charIndex]);
		minBounds.x += data.m_paddedWidthBeforeGlyph;

		Vec2 maxBounds = minBounds + Vec2(textWidth, cellHeight);
		maxBounds.x += data.m_paddedWidthAfterGlyph;

		m_boundingBox.m_maxBounds = maxBounds;
		AABB2 box = AABB2(minBounds, maxBounds);

		//Get the UVs
		SpriteDefenition sd = m_bitmapSpriteSheet->GetSpriteDef((int)printText[charIndex]);
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
void BitmapFont::AddVertsForTextInBox2D( std::vector<Vertex_PCU>& textVerts, const AABB2& box, float cellHeight, const std::string& printText, const Rgba& tintColor /*= Rgba::WHITE*/, float cellAspect /*= 1.f*/, const Vec2& alignment /*= Vec2::ALIGN_CENTERED*/, eTextBoxMode textBoxMode /*= TEXT_BOX_MODE_SHRINK*/, int maxGlyphsToDraw /*= 999999999 */ )
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
void BitmapFont::AddVertsForTextInBox3D( std::vector<Vertex_PCU>& textVerts, const AABB2& box, float cellHeight, const std::string& printText, const Rgba& tintColor /*= Rgba::WHITE*/, float cellAspect /*= 1.f*/, const Vec2& alignment /*= Vec2::ALIGN_CENTERED*/, eTextBoxMode mode /*= TEXT_BOX_MODE_SHRINK*/, int maxGlyphsToDraw /*= 999999999 */ )
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

