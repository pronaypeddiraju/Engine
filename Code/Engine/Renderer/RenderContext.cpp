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

typedef unsigned int uint;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// D3D11 STUFF
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// Required Headers
#include <d3d11.h>  
#include <DXGI.h>    

// DEBUG STUFF
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

ID3D11Device *gD3DDevice = nullptr;
ID3D11DeviceContext *gD3DContext = nullptr;
IDXGISwapChain *gD3DSwapChain = nullptr;

//For now make a global rtv
ID3D11RenderTargetView *g_rtv;

//------------------------------------------------------------------------------------------------------------------------------
// D3D11 Functions
//------------------------------------------------------------------------------------------------------------------------------

bool RenderContext::D3D11Setup( void* hwndVoid )
{
	HWND hwnd = (HWND)hwndVoid;

	// Creation Flags
	// For options, see;
	// https://www.google.com/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#safe=off&q=device_flags+%7C%3D+D3D11_CREATE_DEVICE_DEBUG%3B
	uint device_flags = 0U;
#if defined(RENDER_DEBUG)
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;

	// This flag fails unless we' do 11.1 (which we're not), and we query that
	// the adapter support its (which we're not).  Just here to let you know it exists.
	// device_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE; 
#endif

	// Setup our Swap Chain
	// For options, see;
	// https://www.google.com/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#safe=off&q=DXGI_SWAP_CHAIN_DESC

	DXGI_SWAP_CHAIN_DESC swap_desc;
	memset( &swap_desc, 0, sizeof(swap_desc) );

	// fill the swap chain description struct
	swap_desc.BufferCount = 2;                                    // two buffers (one front, one back?)

	swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT       // how swap chain is to be used
		| DXGI_USAGE_BACK_BUFFER;                                  
	swap_desc.OutputWindow = hwnd;                                // the window to be copied to on present
	swap_desc.SampleDesc.Count = 1;                               // how many multisamples (1 means no multi sampling)

	RECT client_rect; 
	::GetClientRect( hwnd, &client_rect ); 
	uint width = client_rect.right - client_rect.left; 
	uint height = client_rect.bottom - client_rect.top; 

	// Default options.
	swap_desc.Windowed = TRUE;                                    // windowed/full-screen mode
	swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	swap_desc.BufferDesc.Width = width;
	swap_desc.BufferDesc.Height = height;


	// Actually Create
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain( nullptr, // Adapter, if nullptr, will use adapter window is primarily on.
		D3D_DRIVER_TYPE_HARDWARE,  // Driver Type - We want to use the GPU (HARDWARE)
		nullptr,                   // Software Module - DLL that implements software mode (we do not use)
		device_flags,              // device creation options
		nullptr,                   // feature level (use default)
		0U,                        // number of feature levels to attempt
		D3D11_SDK_VERSION,         // SDK Version to use
		&swap_desc,                // Description of our swap chain
		&gD3DSwapChain,            // Swap Chain we're creating
		&gD3DDevice,               // [out] The device created
		nullptr,                   // [out] Feature Level Acquired
		&gD3DContext );            // Context that can issue commands on this pipe.

								   // SUCCEEDED & FAILED are macros provided by Windows to checking
								   // the results.  Almost every D3D call will return one - be sure to check it.
	return SUCCEEDED(hr);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void RenderContext::D3D11Cleanup()
{
	DX_SAFE_RELEASE(gD3DSwapChain);
	DX_SAFE_RELEASE(gD3DContext);
	DX_SAFE_RELEASE(gD3DDevice);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void RenderContext::DemoRender()
{
	// Get the back buffer
	ID3D11Texture2D *back_buffer = nullptr;
	gD3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);

	// Get a render target view of this
	// NOTE:  This could be cached off and stored instead of creating
	// a new one each frame.  It is fairly cheap to do though.
	ID3D11RenderTargetView *rtv = nullptr;
	gD3DDevice->CreateRenderTargetView( back_buffer, nullptr, &rtv );
	DX_SAFE_RELEASE( back_buffer ); // I'm done using this - so release my hold on it (does not destroy it!)

									// Clear the buffer.
	float clear_color[4] = { 0.004f, 0.475f, 0.435f, 1.0f };
	gD3DContext->ClearRenderTargetView( rtv, clear_color );
	DX_SAFE_RELEASE( rtv ); // done with the view - can release it (if you save it frame to frame, skip this step)

							// We're done rendering, so tell the swap chain they can copy the back buffer to the front (desktop/window) buffer
	gD3DSwapChain->Present( 0, // Sync Interval, set to 1 for VSync
		0 );                    // Present flags, see;
								// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509554(v=vs.85).aspx
}

//------------------------------------------------------------------------------------------------------------------------------
// Render Context Methods
//------------------------------------------------------------------------------------------------------------------------------

RenderContext* g_renderContext = nullptr;

RenderContext::RenderContext(void* windowHandle)
{
	D3D11Setup(windowHandle);
}

void RenderContext::Startup()
{
	//DemoRender();
}

void RenderContext::SetBlendMode(BlendMode blendMode)
{
	UNUSED(blendMode);
	GUARANTEE_RECOVERABLE(false, "Reached Set blend mode in RC");
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
	// Get the back buffer
	ID3D11Texture2D *back_buffer = nullptr;
	gD3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);

	// Get a render target view of this
	// NOTE:  This could be cached off and stored instead of creating
	// a new one each frame.  It is fairly cheap to do though.
	g_rtv = nullptr;
	gD3DDevice->CreateRenderTargetView( back_buffer, nullptr, &g_rtv );
	DX_SAFE_RELEASE( back_buffer ); // I'm done using this - so release my hold on it (does not destroy it!)

}

void RenderContext::EndFrame()
{
	DX_SAFE_RELEASE( g_rtv ); // done with the view - can release it (if you save it frame to frame, skip this step)

							// We're done rendering, so tell the swap chain they can copy the back buffer to the front (desktop/window) buffer
	gD3DSwapChain->Present( 0, // Sync Interval, set to 1 for VSync
		0 );                    // Present flags, see;
								// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509554(v=vs.85).aspx
}

void RenderContext::Shutdown()
{
	D3D11Cleanup();
}


void RenderContext::BindTexture( Texture* texture )
{
	UNUSED(texture);
	GUARANTEE_RECOVERABLE(false, "Reached Bind Texture");
}

void RenderContext::ClearScreen( const Rgba & clearColor )
{
	// Clear the buffer.
	float clear_color[4] = { clearColor.r, clearColor.g, clearColor.b, clearColor.a};
	gD3DContext->ClearRenderTargetView( g_rtv, clear_color );
}


void RenderContext::BeginCamera( const Camera &camera )
{
	UNUSED(camera);
	//GUARANTEE_RECOVERABLE(false, "Reached Begin Camera");
	
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
	//GUARANTEE_RECOVERABLE(false, "Reached End Camera");

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

