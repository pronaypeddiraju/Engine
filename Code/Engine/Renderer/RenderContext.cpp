#include "RenderContext.hpp"
#include "Engine/Math/Vec2.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Renderer/Texture.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places

/*

NUKED!

//-----------------------------------------------------------------------------------------------
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library

#define UNUSED(x) (void)(x);
*/


RenderContext* g_renderContext = nullptr;



RenderContext::RenderContext(void* windowHandle)
{

}

void RenderContext::Startup()
{


	GUARANTEE_RECOVERABLE(false, "Reached Startup in RC");

	/*

	NUKED!

	// #SD1ToDo: move all OpenGL functions (including those below) to RenderContext.cpp (only!)
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	*/
	
}

void RenderContext::SetBlendMode(BlendMode blendMode)
{
	UNUSED(blendMode);
	GUARANTEE_RECOVERABLE(false, "Reached Set blend mode in RC");

	/*

	NUKED!

	if(blendMode == BLEND_MODE_ALPHA)
	{
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}
	else
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}

	*/

}

Texture* RenderContext::CreateTextureFromFile( const char* imageFilePath )
{
	UNUSED(imageFilePath);
	GUARANTEE_RECOVERABLE(false, "Reached Create Texture From File");

	return nullptr;
}

BitmapFont* RenderContext::CreateBitmapFontFromFile( std::string bitmapName )
{
	std::string filePath = "Data/Fonts/" + bitmapName + ".png";
	Texture* bitmapTexture = CreateTextureFromFile(filePath.c_str());

	BitmapFont* newFont = new BitmapFont(bitmapName, bitmapTexture);

	m_loadedFonts[bitmapName] = newFont;
	return newFont;
}

void RenderContext::BeginFrame()
{
}

void RenderContext::EndFrame()
{
	//Perform swap buffers here
}

void RenderContext::Shutdown()
{
}


void RenderContext::BindTexture( Texture* texture )
{
	UNUSED(texture);
	GUARANTEE_RECOVERABLE(false, "Reached Bind Texture");

	/*

	NUKED!

	if(texture)
	{
		//Enabling the GL stuff to use textures
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	}
	else
	{
		//No texture? Disable the GL stuff for textures
		glDisable(GL_TEXTURE_2D);
	}
	*/
}

void RenderContext::ClearScreen( const Rgba & clearColor )
{
	UNUSED(clearColor);
	GUARANTEE_RECOVERABLE(false, "Reached Clear Screen");

	/*

	NUKED!

	// Clear all screen (backbuffer) pixels to black
	glClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
	glClear( GL_COLOR_BUFFER_BIT );
	*/

}


void RenderContext::BeginCamera( const Camera &camera )
{
	UNUSED(camera);
	GUARANTEE_RECOVERABLE(false, "Reached Begin Camera");
	/*

	NUKED!

	//UNUSED(camera);
	//Add Camera code here
	// Establish a 2D (orthographic) drawing coordinate system: (0,0) bottom-left to (100,100) top-right
	// #SD1ToDo: This will be replaced by a call to g_renderer->BeginView( m_worldView ); or similar
	glLoadIdentity();
	glOrtho( camera.GetOrthoBottomLeft().x, camera.GetOrthoTopRight().x, camera.GetOrthoBottomLeft().y, camera.GetOrthoTopRight().y , 0.f, 1.f );
	*/
}

void RenderContext::EndCamera( const Camera &camera )
{
	UNUSED(camera);
	GUARANTEE_RECOVERABLE(false, "Reached End Camera");

	//Destroy the camera here
}

void RenderContext::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes )
{
	UNUSED(numVertexes);
	UNUSED(vertexes);
	GUARANTEE_RECOVERABLE(false, "Reached Set blend mode in RC");

	/*

	NUKED!

	glBegin( GL_TRIANGLES );
	{
		for (int i = 0; i < numVertexes; i++)
		{
			glColor4f(vertexes[i].m_color.r, vertexes[i].m_color.g, vertexes[i].m_color.b, vertexes[i].m_color.a);
			glTexCoord2f(vertexes[i].m_uvTexCoords.x, vertexes[i].m_uvTexCoords.y);
			glVertex3f(vertexes[i].m_position.x, vertexes[i].m_position.y, vertexes[i].m_position.z);
		}
		
	}
	glEnd();
	*/
}



void RenderContext::DrawVertexArray( const std::vector<Vertex_PCU>& vertexes )
{
	DrawVertexArray( static_cast<int>(vertexes.size()), &vertexes[0]);
}

/*
//-----------------------------------------------------------------------------------------------
// Some simple OpenGL example drawing code.
// This is the graphical equivalent of printing "Hello, world."
// #SD1ToDo: Remove this function and replace it with TheApp::Render()
// #SD1ToDo: Move all OpenGL code to RenderContext.cpp (only)
//
void RenderContext::DebugRender()
{
	// Draw a line from the bottom-left corner of the screen (0,0) to the center of the screen (50,50)
	// #SD1ToDo: Move all OpenGL code into RenderContext.cpp (only); call g_renderer->DrawVertexArray() instead
	// First triangle (3 vertexes, each preceded by a color)
	
	// Establish a 2D (orthographic) drawing coordinate system: (0,0) bottom-left to (100,100) top-right
	// #SD1ToDo: This will be replaced by a call to g_renderer->BeginView( m_worldView ); or similar
	glLoadIdentity();
	glOrtho( 0.f, 16 * 1.77f, 0.f, 30, 0.f, 1.f );
	//glOrtho(0.f, WORLD_WIDTH, 0.f, WORLD_HEIGHT, 0.f, 1.f);
	glBegin( GL_TRIANGLES );
	{
		glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );
		glVertex2f( 0.f, 1.f );

		glColor4f( 0.0f, 1.0f, 0.0f, 1.0f );
		glVertex2f( 8.f, 2.f );

		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		glVertex2f( 3.f, 5.f );

		// Second triangle (3 vertexes, all one color)
		glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
		glVertex2f( 4.f, 3.f );
		glVertex2f( 9.f, 6.f );
		glVertex2f( 7.f, 8.f );
	}
	glEnd();

	
}
*/

Texture* RenderContext::CreateOrGetTextureFromFile(const char* imageFilePath)
{
	std::map<std::string, Texture*>::const_iterator requestedTexture = m_loadedTextures.find(imageFilePath);
	if(requestedTexture != m_loadedTextures.end())
	{
		//Texture requested exists in the map
		return requestedTexture->second;
	}
	else
	{
		//Create the new tex
		Texture* texture = CreateTextureFromFile(imageFilePath);
		return texture;
	}
}

BitmapFont* RenderContext::CreateOrGetBitmapFontFromFile(std::string bitmapName)
{
	std::map<std::string, BitmapFont*>::const_iterator requestedFont = m_loadedFonts.find(bitmapName);
	if(requestedFont != m_loadedFonts.end())
	{
		//Font requested exists in the map
		return requestedFont->second;
	}
	else
	{
		//Create new font
		BitmapFont* font = CreateBitmapFontFromFile(bitmapName);
		return font;
	}
}


/*
//------------------------------------------------------------------------------------------------------------------------------
Texture* RenderContext::CreateTextureFromFile( const char* imageFilePath )
{
	int imageTexelSizeX = 0; // Filled in for us to indicate image width
	int imageTexelSizeY = 0; // Filled in for us to indicate image height
	int numComponents = 0; // Filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)
	
	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );
	GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ));
	GUARANTEE_OR_DIE( numComponents >= 3 && numComponents <= 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );	

	Texture* newTexture = new Texture();
	//This should be your intVec2 data!
	newTexture->m_dimensions.x = imageTexelSizeX;
	newTexture->m_dimensions.y = imageTexelSizeY;

	// Enable OpenGL texturing
	glEnable( GL_TEXTURE_2D );

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	// Ask OpenGL for an unused texName (ID number) to use for this texture
	glGenTextures( 1, (GLuint*) &newTexture->m_textureID );

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture( GL_TEXTURE_2D, newTexture->m_textureID );

	// Set texture clamp vs. wrap (repeat) default settings
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP ); // GL_CLAMP or GL_REPEAT
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP ); // GL_CLAMP or GL_REPEAT

																   // Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

																		// Pick the appropriate OpenGL format (RGB or RGBA) for this texel data
	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if( numComponents == 3 )
	{
		bufferFormat = GL_RGB;
	}
	GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; allows us to translate into a different texture format as we upload to OpenGL

	// Upload the image texel data (raw pixels bytes) to OpenGL under this textureID
	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D,		// Creating this as a 2d texture
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		imageTexelSizeX,	// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
		imageTexelSizeY,	// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
		0,					// Border size, in texels (must be 0 or 1, recommend 0)
		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
		imageData );		// Address of the actual pixel data bytes/buffer in system memory

	// Free the image data now that we've sent a copy of it down to the GPU to be stored in video memory
	stbi_image_free( imageData );

	//Add the texture to the map along with it's file path
	m_loadedTextures[imageFilePath] = newTexture;

	return newTexture;
}
*/

