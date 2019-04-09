//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Rgba.hpp"

#pragma warning( disable: 4100) //Unreferenced formal parameter
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"

//------------------------------------------------------------------------------------------------------------------------------
Image::Image( const char* imageFilePath )
{
	int imageTexelSizeX = 0; // Filled in for us to indicate image width
	int imageTexelSizeY = 0; // Filled in for us to indicate image height
	int numComponents = 0; // Filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 4; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

	//D3D prefers it to be (0,0) at the bottom
	//stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	
	m_imageRawData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );

	m_dimensions.x = imageTexelSizeX;
	m_dimensions.y = imageTexelSizeY;

	int numTexels = imageTexelSizeX * imageTexelSizeY;	
	m_texelRepository.resize(numTexels);

	if(numComponents == 3)
	{
		//we have RGB data
		for(int texelIndex = 0; texelIndex < numTexels; texelIndex++)
		{
			int redByteIndex = texelIndex * numComponents;
			int greenByteIndex = redByteIndex + 1;
			int blueByteIndex = redByteIndex + 2;

			unsigned char redByte = m_imageRawData[redByteIndex];
			unsigned char greenByte = m_imageRawData[greenByteIndex];
			unsigned char blueByte = m_imageRawData[blueByteIndex];

			m_texelRepository[texelIndex] = new Rgba();
			m_texelRepository[texelIndex]->SetFromBytes(redByte, greenByte, blueByte);
		}
	}
	else
	{
		//WE have RGBA data
		for(int texelIndex = 0; texelIndex < numTexels; texelIndex++)
		{
			int redByteIndex = texelIndex * numComponents;
			int greenByteIndex = redByteIndex + 1;
			int blueByteIndex = redByteIndex + 2;
			int alphaByteIndex = redByteIndex + 3;

			unsigned char redByte = m_imageRawData[redByteIndex];
			unsigned char greenByte = m_imageRawData[greenByteIndex];
			unsigned char blueByte = m_imageRawData[blueByteIndex];
			unsigned char alphaByte = m_imageRawData[alphaByteIndex];

			m_texelRepository[texelIndex] = new Rgba();
			m_texelRepository[texelIndex]->SetFromBytes(redByte, greenByte, blueByte, alphaByte);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
Image::Image( const Rgba& color, const int width /*= 1*/, const int height /*= 1*/ )
{
	//Resize vector size to the number of texels
	int numTexels = width * height;

	m_dimensions.x = width;
	m_dimensions.y = height;

	m_texelRepository.resize(numTexels);

	for(int texelIndex = 0; texelIndex < numTexels; texelIndex++)
	{
		int redByteIndex = texelIndex;
		int greenByteIndex = redByteIndex + 1;
		int blueByteIndex = redByteIndex + 2;
		int alphaByteIndex = redByteIndex + 3;

		unsigned char redByte = m_imageRawData[redByteIndex];
		unsigned char greenByte = m_imageRawData[greenByteIndex];
		unsigned char blueByte = m_imageRawData[blueByteIndex];
		unsigned char alphaByte = m_imageRawData[alphaByteIndex];

		*m_texelRepository[texelIndex] = color;
		m_texelRepository[texelIndex]->SetFromBytes(redByte, greenByte, blueByte, alphaByte);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
Image::~Image()
{
	stbi_image_free(m_imageRawData);
	m_imageRawData = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
const Rgba& Image::GetTexelColor( const IntVec2& texelCoordinates ) const
{
	//Get index from the coordinates
	int texelIndex = texelCoordinates.x + texelCoordinates.y * m_dimensions.x;
	return *m_texelRepository[texelIndex];	
}

//------------------------------------------------------------------------------------------------------------------------------
const Rgba& Image::GetTexelColor(int xCoord, int yCoord) const
{
	int texelIndex = xCoord + yCoord * m_dimensions.x;
	return *m_texelRepository[texelIndex];	
}

//------------------------------------------------------------------------------------------------------------------------------
const IntVec2& Image::GetImageDimensions() const
{
	return m_dimensions;
}

//------------------------------------------------------------------------------------------------------------------------------
const std::string& Image::GetImageFilePath() const
{
	return m_imageFilePath;
}

//------------------------------------------------------------------------------------------------------------------------------
const uint Image::GetBytesPerPixel() const
{
	//For our purposes it is always R8G8B8A8 so that's 4 bytes
	return 4;
}

//------------------------------------------------------------------------------------------------------------------------------
const void* Image::GetImageBuffer() const
{
	return m_imageRawData;
}

//------------------------------------------------------------------------------------------------------------------------------
void Image::SetTexelColor( int xCoord, int yCoord, const Rgba& setColor )
{
	int texelIndex = xCoord + yCoord * m_dimensions.x;
	*m_texelRepository[texelIndex] = setColor;
}

//------------------------------------------------------------------------------------------------------------------------------
void Image::SetTexelColor( const IntVec2& texelCoordinates, const Rgba& setColor )
{
	int texelIndex = texelCoordinates.x + texelCoordinates.y * m_dimensions.x;
	*m_texelRepository[texelIndex] = setColor;
}