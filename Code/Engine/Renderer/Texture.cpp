//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include <d3d11.h>

//------------------------------------------------------------------------------------------------------------------------------
static uint DXBindFromUsage( uint usage ) 
{
	uint binds = 0U; 
	if (usage & TEXTURE_USAGE_TEXTURE_BIT) {
		binds |= D3D11_BIND_SHADER_RESOURCE; 
	}
	if (usage & TEXTURE_USAGE_COLOR_TARGET_BIT) {
		binds |= D3D11_BIND_RENDER_TARGET; 
	}
	if (usage & TEXTURE_USAGE_DEPTH_STENCIL_TARGET_BIT) {
		binds |= D3D11_BIND_DEPTH_STENCIL; 
	}

	return binds; 
}

Texture2D::Texture2D( RenderContext *renderContext )
	: Texture(renderContext)
{

}

Texture2D::~Texture2D()
{
	DX_SAFE_RELEASE(m_handle);
}

bool Texture2D::LoadTextureFromFile( std::string const &filename, bool isFont ) 
{
	std::string path;

	if(!isFont)
	{
		path = IMAGE_PATH + filename;
	}
	else
	{
		path = FONT_PATH + filename + ".png";
	}

	Image image(path.c_str());

	if (image.GetImageDimensions() == IntVec2::ZERO) 
	{
		return false; 
	}

	return LoadTextureFromImage( image ); 
}

//------------------------------------------------------------------------
bool Texture2D::LoadTextureFromImage( Image const &image ) 
{
	// cleanup old resources before creating new one just in case; 
	if(m_handle != nullptr)
	{
		delete m_handle;
		DX_SAFE_RELEASE(m_handle);
	}

	ID3D11Device *dd = m_owner->m_D3DDevice; 

	// If created from image, we'll assume it is only needed
	// as a read-only texture resource (if this is not true, change the
	// signature to take in the option)
	m_textureUsage = TEXTURE_USAGE_TEXTURE_BIT;

	// we are not picking static here because
	// we will eventually want to generate mipmaps,
	// which requires a GPU access pattern to generate.
	m_memoryUsage = GPU_MEMORY_USAGE_GPU; 

	// Setup the Texture Description (what the resource will be like on the GPU)
	D3D11_TEXTURE2D_DESC texDesc;
	memset( &texDesc, 0, sizeof(texDesc) );

	IntVec2 dimensions = image.GetImageDimensions();

	texDesc.Width = dimensions.x;
	texDesc.Height = dimensions.y;
	texDesc.MipLevels = 1; // setting to 0 means there's a full chain (or can generate a full chain)
	texDesc.ArraySize = 1; // only one texture
	texDesc.Usage = RenderBuffer::DXUsageFromMemoryUsage(m_memoryUsage);  // loaded from image - probably not changing
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;            // if you support different image types  - this could change!  
	texDesc.BindFlags = DXBindFromUsage(m_textureUsage);   // only allowing rendertarget for mipmap generation
	texDesc.CPUAccessFlags = 0U;                            // Determines how I can access this resource CPU side 
	texDesc.MiscFlags = 0U;  

	// If Multisampling - set this up.
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	// Setup Initial Data
	// pitch is how many bytes is a single row of pixels;  
	uint pitch = dimensions.x * image.GetBytesPerPixel(); // 4 bytes for an R8G8B8A8 format;  Just sub in four if your image is always 4 bytes per channel
	D3D11_SUBRESOURCE_DATA data;
	memset( &data, 0, sizeof(data) );
	data.pSysMem = image.GetImageBuffer();
	data.SysMemPitch = pitch;

	// Actually create it
	ID3D11Texture2D *tex2D = nullptr; 
	HRESULT hr = dd->CreateTexture2D( &texDesc,
		&data, 
		&tex2D );

	if (SUCCEEDED(hr)) 
	{
		// save off the info; 
		m_dimensions = dimensions; 
		m_handle = tex2D;
		// TODO later assigment, generate mips if option is set; 

		return true; 

	} 
	else 
	{
		ASSERT( tex2D == nullptr ); // should be, just like to have the postcondition; 
		return false; 
	}
}

//------------------------------------------------------------------------
TextureView2D* Texture2D::CreateTextureView2D() const
{
	// if we don't have a handle, we can't create a view, so return nullptr
	ASSERT_RETURN_VALUE( m_handle != nullptr, nullptr );

	// 2D - we will want to eventually create specific views of a texture
	// and will want ot fill out a D3D11_SHADER_RESOURCE_VIEW_DESC, but for now
	// we'll just do the default thing (nullptr)

	// get our device - since we're creating a resource
	ID3D11Device *dev = m_owner->m_D3DDevice; 
	ID3D11ShaderResourceView *srv = nullptr; 
	dev->CreateShaderResourceView( m_handle, nullptr, &srv );

	if (srv != nullptr) {
		// Awesome, we have one
		TextureView2D *view = new TextureView2D();

		// give it the handle to the srv (we do not AddRef, 
		// but are instead just handing this off)
		view->m_view = srv; 

		// Also let the view hold onto a handle to this texture
		// (so both the texture AND the view are holding onto it)
		// (hence the AddRef)
		m_handle->AddRef(); 
		view->m_source = m_handle; 

		// copy the size over for convenience
		view->m_size = m_dimensions;

		// done - return!
		return view; 
	} 
	else 
	{
		return nullptr; 
	}
}

Texture::Texture( RenderContext *renderContext )
{
	m_owner = renderContext;
}

Texture::~Texture()
{
	DX_SAFE_RELEASE(m_handle);
}
