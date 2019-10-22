//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Rgba.hpp"

#pragma warning( disable: 4100) //Unreferenced formal parameter
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"
#include "ThirdParty/stb/stb_image_write.h"

//------------------------------------------------------------------------------------------------------------------------------
Image::Image( const char* imageFilePath )
{
	m_imageFilePath = imageFilePath;
	m_imageRawData = nullptr;

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

			m_texelRepository[texelIndex] = Rgba();
			m_texelRepository[texelIndex].SetFromBytes(redByte, greenByte, blueByte);
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

			m_texelRepository[texelIndex] = Rgba();
			m_texelRepository[texelIndex].SetFromBytes(redByte, greenByte, blueByte, alphaByte);
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

	m_imageRawData = (unsigned char*)malloc(numTexels * GetBytesPerPixel());
	
	for(int texelIndex = 0; texelIndex < numTexels; texelIndex++)
	{
		m_texelRepository[texelIndex] = color;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
Image::Image()
{
	
}

//------------------------------------------------------------------------------------------------------------------------------
Image::Image(const int width /*= 1*/, const int height /*= 1*/)
{
	//Resize vector size to the number of texels
	int numTexels = width * height;

	m_dimensions.x = width;
	m_dimensions.y = height;

	m_texelRepository.resize(numTexels);

	m_imageRawData = (unsigned char*)malloc(numTexels * GetBytesPerPixel());
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
	return m_texelRepository[texelIndex];	
}

//------------------------------------------------------------------------------------------------------------------------------
const Rgba& Image::GetTexelColor(int xCoord, int yCoord) const
{
	int texelIndex = xCoord + yCoord * m_dimensions.x;
	return m_texelRepository[texelIndex];	
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
void* Image::GetWritableImageBuffer()
{
	return m_imageRawData;
}

//------------------------------------------------------------------------------------------------------------------------------
size_t Image::GetImageSizeAsSizeT() const
{
	return m_dimensions.x * m_dimensions.y * GetBytesPerPixel();
}

//------------------------------------------------------------------------------------------------------------------------------
void* Image::GetRawPointerToRow(uint rowNum)
{
	return m_imageRawData + rowNum * (m_dimensions.y * GetBytesPerPixel());
}

//------------------------------------------------------------------------------------------------------------------------------
void Image::SetTexelColor( int xCoord, int yCoord, const Rgba& setColor )
{
	int texelIndex = xCoord + yCoord * m_dimensions.x;
	m_texelRepository[texelIndex] = setColor;
	
	//Set the corresponding byte 
	int redByteIndex = texelIndex * 4;
	int greenByteIndex = redByteIndex + 1;
	int blueByteIndex = redByteIndex + 2;
	int alphaByteIndex = redByteIndex + 3;

	m_imageRawData[redByteIndex] = (unsigned char)(setColor.r * 255.f);
	m_imageRawData[greenByteIndex] = (unsigned char)(setColor.g * 255.f);
	m_imageRawData[blueByteIndex] = (unsigned char)(setColor.b * 255.f);
	m_imageRawData[alphaByteIndex] = (unsigned char)(setColor.a * 255.f);
}

//------------------------------------------------------------------------------------------------------------------------------
void Image::SetTexelColor( const IntVec2& texelCoordinates, const Rgba& setColor )
{
	int texelIndex = texelCoordinates.x + texelCoordinates.y * m_dimensions.x;
	m_texelRepository[texelIndex] = setColor;

	//Set the corresponding byte 
	int redByteIndex = texelIndex * 4;
	int greenByteIndex = redByteIndex + 1;
	int blueByteIndex = redByteIndex + 2;
	int alphaByteIndex = redByteIndex + 3;

	m_imageRawData[redByteIndex] = (unsigned char)(setColor.r * 255.f);
	m_imageRawData[greenByteIndex] = (unsigned char)(setColor.g * 255.f);
	m_imageRawData[blueByteIndex] = (unsigned char)(setColor.b * 255.f);
	m_imageRawData[alphaByteIndex] = (unsigned char)(setColor.a * 255.f);
}

/*
void Image::operator=(const Image& copyFrom)
{
	m_imageFilePath = copyFrom.m_imageFilePath;
	m_dimensions = copyFrom.m_dimensions;
	m_texelRepository = copyFrom.m_texelRepository;
	m_imageRawData = copyFrom.m_imageRawData;
}
*/