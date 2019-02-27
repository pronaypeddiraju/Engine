#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/Vec2.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Renderer/DepthStencilTargetView.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/GPUMesh.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places

/*

NUKED!

//-----------------------------------------------------------------------------------------------
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library

#define UNUSED(x) (void)(x);
*/

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
#if defined(_DEBUG)
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
		&m_D3DSwapChain,            // Swap Chain we're creating
		&m_D3DDevice,               // [out] The device created
		nullptr,                   // [out] Feature Level Acquired
		&m_D3DContext );            // Context that can issue commands on this pipe.

								   // SUCCEEDED & FAILED are macros provided by Windows to checking
								   // the results.  Almost every D3D call will return one - be sure to check it.
	return SUCCEEDED(hr);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void RenderContext::D3D11Cleanup()
{
	ID3D11Debug* debugObject = nullptr;
	HRESULT hr = m_D3DDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&debugObject);
	
	DX_SAFE_RELEASE(m_defaultRasterState);
	DX_SAFE_RELEASE(m_D3DSwapChain);
	DX_SAFE_RELEASE(m_D3DContext);
	DX_SAFE_RELEASE(m_D3DDevice);

	if(SUCCEEDED(hr))
	{
		//Uncomment when debugging
		debugObject->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		DX_SAFE_RELEASE(debugObject);
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void RenderContext::DemoRender()
{
	// Get the back buffer
	ID3D11Texture2D *back_buffer = nullptr;
	m_D3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);

	// Get a render target view of this
	// NOTE:  This could be cached off and stored instead of creating
	// a new one each frame.  It is fairly cheap to do though.
	ID3D11RenderTargetView *rtv = nullptr;
	m_D3DDevice->CreateRenderTargetView( back_buffer, nullptr, &rtv );
	DX_SAFE_RELEASE( back_buffer ); // I'm done using this - so release my hold on it (does not destroy it!)

									// Clear the buffer.
	float clear_color[4] = { 0.004f, 0.475f, 0.435f, 1.0f };
	m_D3DContext->ClearRenderTargetView( rtv, clear_color );
	DX_SAFE_RELEASE( rtv ); // done with the view - can release it (if you save it frame to frame, skip this step)

							// We're done rendering, so tell the swap chain they can copy the back buffer to the front (desktop/window) buffer
	m_D3DSwapChain->Present( 0, // Sync Interval, set to 1 for VSync
		0 );                    // Present flags, see;
								// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509554(v=vs.85).aspx
}

//------------------------------------------------------------------------------------------------------------------------------
// Render Context Methods
//------------------------------------------------------------------------------------------------------------------------------

RenderContext* g_renderContext = nullptr;

RenderContext::RenderContext(WindowContext* window)
{
	D3D11Setup(window->m_hwnd);
	Startup();
}

RenderContext::~RenderContext()
{
	Shutdown();
}

void RenderContext::Startup()
{
	// Change default behavior to be counter-clockwise is front-face to match GL
	// (plus most meshes are built with this convention)
	CreateAndSetDefaultRasterState(); 

	Sampler *point = new Sampler(); 
	point->SetFilterModes( FILTER_MODE_LINEAR, FILTER_MODE_POINT );  // still min linear - to get blending as it gets smaller; 
	m_cachedSamplers[SAMPLE_MODE_POINT] = point; 

	Sampler *linear = new Sampler(); 
	linear->SetFilterModes( FILTER_MODE_LINEAR, FILTER_MODE_LINEAR ); 
	m_cachedSamplers[SAMPLE_MODE_LINEAR] = linear; 

	PremakeDefaults();

	ModelBufferT buffer; 
	buffer.ModelMatrix = Matrix44::IDENTITY; 

	m_modelBuffer = new UniformBuffer( this ); 
	m_modelBuffer->CopyCPUToGPU( &buffer, sizeof(buffer) ); 
}

void RenderContext::PremakeDefaults()
{
	// premake defaults 
	std::string name = "WHITE.png";
	
	TextureView* white  = GetOrCreateTextureViewFromFile(name);
	m_loadedTextures["WHITE"] = white;
	
}

void RenderContext::SetBlendMode(eBlendMode blendMode)
{
	if(m_currentShader == nullptr)
	{
		ERROR_AND_DIE("No active shader to set blend mode");
	}
	else
	{
		m_currentShader->m_blendMode = blendMode;
	}
}

BitmapFont* RenderContext::CreateBitmapFontFromFile(const std::string& bitmapName)
{
	//std::string filePath = "Data/Fonts/" + bitmapName + ".png";
	TextureView* bitmapTexture = GetOrCreateTextureViewFromFile(bitmapName.c_str(), true);

	BitmapFont* newFont = new BitmapFont(bitmapName, bitmapTexture);

	m_loadedFonts[bitmapName] = newFont;
	return newFont;
}

Shader* RenderContext::CreateShaderFromFile(const std::string& fileName)
{
	char* outData = nullptr;
	unsigned long bufferSize = CreateFileBuffer( fileName, &outData); 

	Shader* shader = new Shader();
	
	shader->m_vertexStage.LoadShaderFromSource(this, fileName, outData, bufferSize, SHADER_STAGE_VERTEX );
	shader->m_pixelStage.LoadShaderFromSource(this, fileName, outData, bufferSize, SHADER_STAGE_FRAGMENT);
	
	//Delete your outData!
	delete[] outData;

	m_loadedShaders[fileName] = shader;
	return shader;
}

void RenderContext::CreateAndSetDefaultRasterState()
{
	D3D11_RASTERIZER_DESC desc; 

	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = TRUE; // the only reason we're doing this; 
	desc.DepthBias = 0U; 
	desc.DepthBiasClamp = 0.0f; 
	desc.SlopeScaledDepthBias = 0.0f; 
	desc.DepthClipEnable = TRUE; 
	desc.ScissorEnable = FALSE; 
	desc.MultisampleEnable = FALSE; 
	desc.AntialiasedLineEnable = FALSE; 

	// ID3D11RasterizerState *m_defaultRasterState; 
	m_D3DDevice->CreateRasterizerState( &desc, &m_defaultRasterState );
	m_D3DContext->RSSetState( m_defaultRasterState); 
}

void RenderContext::BeginFrame()
{
	// Get the back buffer
	ID3D11Texture2D *back_buffer = nullptr;
	m_D3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);

	// Okay, we have an rtv, store it in our own class
	m_FrameBuffer_ColorTargetView = new ColorTargetView();
	m_FrameBuffer_ColorTargetView->CreateForInternalTexture( *back_buffer, *m_D3DDevice ); 

	//ColorTargetView holds a reference to the back_buffer so we can now release it from this function
	DX_SAFE_RELEASE( back_buffer ); // I'm done using this - so release my hold on it (does not destroy it!)

}

ColorTargetView* RenderContext::GetFrameColorTarget()
{
	return m_FrameBuffer_ColorTargetView;
}

void RenderContext::EndFrame()
{
	// We're done rendering, so tell the swap chain they can copy the back buffer to the front (desktop/window) buffer
	m_D3DSwapChain->Present( 0, // Sync Interval, set to 1 for VSync
		0 );                    // Present flags, see;
								// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509554(v=vs.85).aspx

	//Free up the color target view or we have a leak in memory 
	delete m_FrameBuffer_ColorTargetView;
	m_FrameBuffer_ColorTargetView = nullptr;

	//Note: Ask Forseth what he means by reusing this by releasing it every frame
}

void RenderContext::Shutdown()
{
	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	delete m_immediateUBO;
	m_immediateUBO = nullptr;

	delete m_FrameBuffer_ColorTargetView;
	m_FrameBuffer_ColorTargetView = nullptr;

	delete m_defaultDepthStencilView;
	m_defaultDepthStencilView = nullptr;

	delete m_modelBuffer;
	m_modelBuffer = nullptr;

	delete m_immediateMesh;
	m_immediateMesh = nullptr;

	//m_loadedShaders;
	std::map< std::string, Shader*>::iterator shaderIterator;
	std::map< std::string, Shader*>::iterator lastShaderIterator;

	shaderIterator = m_loadedShaders.begin();
	lastShaderIterator = m_loadedShaders.end();
	
	for(shaderIterator; shaderIterator != lastShaderIterator; shaderIterator++)
	{
		delete shaderIterator->second;
	}

	//Free all samplers
	int numFilterModes = NUM_FILTER_MODES;

	for(int filterIterator = 0; filterIterator < numFilterModes; filterIterator++)
	{
		delete m_cachedSamplers[filterIterator];
		m_cachedSamplers[filterIterator] = nullptr;
	}

	m_loadedShaders.clear();

	//Clear all Textures
	std::map< std::string, TextureView*>::iterator texIterator;
	std::map< std::string, TextureView*>::iterator lastTexIterator;

	texIterator = m_cachedTextureViews.begin();
	lastTexIterator = m_cachedTextureViews.end();

	for(texIterator; texIterator != lastTexIterator; texIterator++)
	{
		delete texIterator->second;
	}

	m_cachedTextureViews.clear();

	/*
	texIterator = m_loadedTextures.begin();
	lastTexIterator = m_loadedTextures.end();

	for(texIterator; texIterator != lastTexIterator; texIterator++)
	{
		delete texIterator->second;
	}

	m_loadedTextures.clear();
	*/

	D3D11Cleanup();

}

void RenderContext::BindShader( Shader* shader )
{
	//    we only worry about the Vertex and Pixel Shader - but all stages in the Graphics
	//    pipeline our program uses should be set.
	//    <note: I use OpenGL naming, and refer to Pixel Shaders are Fragment Shaders>
	m_D3DContext->VSSetShader(shader->m_vertexStage.m_vs, nullptr, 0U);
	m_D3DContext->PSSetShader(shader->m_pixelStage.m_ps, nullptr, 0U);

	shader->UpdateBlendStateIfDirty(this); 
	float black[] = { 0.0f, 0.0f, 0.0f, 1.0f }; 
	m_D3DContext->OMSetBlendState( shader->m_blendState, // the d3d11 blend state object; 
		black,         // blend factor (some blend options use this) 
		0xffffffff );  // mask (what channels will get blended, this means ALL)  

	m_currentShader = shader;
}

//------------------------------------------------------------------------
// Note:  We have 128 texture slots, and 16 sampler slots
// But for simplicity, the system is designed to have the sampler stored
// with the texture view (so users just have to say "BindTexture").  
// Meaning, this design limits us to 16 textures.  
void RenderContext::BindTextureView( uint slot, TextureView *view )
{
	ID3D11ShaderResourceView *srv = nullptr; 
	if (view != nullptr) 
	{
		srv = view->m_view; 
		m_D3DContext->PSSetShaderResources(slot, 1U, &srv);
	} 
	else 
	{
		// TODO - if view is nullptr, default to something
		// that makes sence for the slot
		// For now - bind a solid "WHITE" texture if slot == 0; 
		if(slot == 0)
		{
			//Bind to white texture
			BindTextureView(0U, "WHITE");
		}
	}

	// You can bind textures to the Vertex stage, but not samplers
	// We're *not* for now since no effect we do at Guildhall requires it, but
	// be aware it is an option; 
	// m_context->VSSetShaderResource( slot, 1U, &srv ); 
}

void RenderContext::BindTextureView( uint slot, std::string const &name )
{
	std::map<std::string, TextureView*>::iterator textureIterator;
	textureIterator = m_loadedTextures.find(name);

	BindTextureView(slot, textureIterator->second);
}

//------------------------------------------------------------------------
void RenderContext::BindSampler( uint slot, Sampler *sampler ) 
{
	if (sampler == nullptr) {
		sampler = m_cachedSamplers[SAMPLE_MODE_DEFAULT]; // bind the default if nothing is set
	}

	// create the dx handle; 
	sampler->CreateStateIfDirty(this);

	ID3D11SamplerState *handle = sampler->GetHandle(); 
	m_D3DContext->PSSetSamplers( slot, 1U, &handle ); 
}

void RenderContext::BindSampler( eSampleMode mode )
{
	BindSampler(0U, m_cachedSamplers[mode]);
}

//------------------------------------------------------------------------
void RenderContext::BindTextureViewWithSampler( uint slot, TextureView *view )
{
	BindTextureView( slot, view ); 

	if (view != nullptr) {
		BindSampler( slot, view->m_sampler );
	} else {
		BindSampler( slot, nullptr ); 
	}
}

void RenderContext::BindTextureViewWithSampler( uint slot, std::string const &name )
{
	std::map< std::string, TextureView* >::iterator textureIterator;
	textureIterator = m_loadedTextures.find(name);

	BindTextureViewWithSampler(slot, textureIterator->second);
}

void RenderContext::BindTextureViewWithSampler( uint slot, TextureView *view, Sampler *sampler )
{
	view->m_sampler = sampler;
	BindTextureViewWithSampler(slot, view);
}

void RenderContext::BindTextureViewWithSampler( uint slot, TextureView *view, eSampleMode mode )
{
	view->m_sampler = m_cachedSamplers[mode];
	BindTextureViewWithSampler(slot, view);
}

void RenderContext::BindTextureViewWithSampler( uint slot, std::string const &name, eSampleMode mode )
{
	std::map<std::string, TextureView*>::iterator texIterator;
	texIterator = m_loadedTextures.find(name);

	BindTextureViewWithSampler(slot, texIterator->second, mode);
}

void RenderContext::BindModelMatrix( Matrix44 const &model ) 
{
	ModelBufferT buffer; 
	buffer.ModelMatrix = model; 

	m_modelBuffer->CopyCPUToGPU( &buffer, sizeof(buffer) ); 
}

void RenderContext::SetModelMatrix( Matrix44 const &modelMatrix )
{
	BindModelMatrix(modelMatrix);
}

//------------------------------------------------------------------------
// (NOTE: This design is fairly different from my Engine, 
// so while I'm fairly sure this should work, if it doesn't, please let me know)
TextureView* RenderContext::GetOrCreateTextureViewFromFile( std::string const &filename, bool isFont )
{
	TextureView* view = nullptr; 

	// normal stuff - if it exists, return it; 
	std::map<std::string, TextureView*>::iterator item = m_cachedTextureViews.find(filename); 
	if (item != m_cachedTextureViews.end()) 
	{
		return item->second; 
	} 

	Texture2D *tex = new Texture2D(this); 

	if (!tex->LoadTextureFromFile( filename, isFont))
	{
		delete tex;

		// optional -> store a null at this spot so we don't 
		// bother trying to reload it later; 
		m_cachedTextureViews[filename] = nullptr;

		return nullptr;

	}
	else 
	{
		// create the view
		view = tex->CreateTextureView2D(); 

		// THIS SHOULD BE FINE - the view should hold onto the D3D11 resource;
		// (I'm not 100% on this though, so if not, please Slack me)
		delete tex;  

		m_cachedTextureViews[filename] = view; 

		return view; 
	}
}

void RenderContext::Draw( uint vertexCount, uint byteOffset )
{
	// **NEW** - before a draw can happen, 
	// We need to describe the input to the shader
	// TODO: only create an input layout if the vertex type changes; 
	// TODO: When different vertex types come on-line, look at the current bound
	//       input streams (VertexBuffer) for the layout
	
	bool result =  m_currentShader->CreateInputLayoutForVertexPCU(); 
	
	// TODO: m_currentShader->CreateInputLayoutFor( VertexPCU::LAYOUT ); 

	//A02 Implementation
	if (result) 
	{
		m_D3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_D3DContext->IASetInputLayout( m_currentShader->m_inputLayout );
		m_D3DContext->Draw( vertexCount, byteOffset ); 
	} 
	else 
	{
		// error/warning
		ERROR_AND_DIE("Could not create input layout!");
	}
}

void RenderContext::DrawIndexed( uint indexCount, uint vertexCount, uint byteOffset )
{
	bool result =  m_currentShader->CreateInputLayoutForVertexPCU(); 

	// TODO: m_currentShader->CreateInputLayoutFor( VertexPCU::LAYOUT ); 

	//A02 Implementation
	if (result) 
	{
		m_D3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_D3DContext->IASetInputLayout( m_currentShader->m_inputLayout );
		m_D3DContext->Draw( vertexCount, byteOffset ); 
	} 
	else 
	{
		// error/warning
		ERROR_AND_DIE("Could not create input layout!");
	}

	// Draw
	m_D3DContext->DrawIndexed( indexCount, 
		0,       // elem offset 
		0 );     // vert offset 
}

void RenderContext::ClearColorTargets( const Rgba& clearColor )
{
	// Clear the buffer.
	float clearColorArray[4] = { clearColor.r, clearColor.g, clearColor.b, clearColor.a};
	m_D3DContext->ClearRenderTargetView( m_currentCamera->m_colorTargetView->m_renderTargetView, clearColorArray);
}


void RenderContext::ClearDepthStencilTarget( float depth /*= 1.0f*/, uint8_t stencil /*= 0U */ )
{
	ID3D11DepthStencilView *dsv = nullptr; 
	dsv = m_currentCamera->m_depthStencilView->m_renderTargetView; 
	m_D3DContext->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil ); 
}

void RenderContext::BindVertexStream( VertexBuffer *vbo )
{
	// Bind the input stream; 
	uint stride = sizeof(Vertex_PCU);
	uint offset = 0U;
	m_D3DContext->IASetVertexBuffers( 0,    // Start slot index
		1,                            // Number of buffers we're binding
		&vbo->m_handle, // Array of buffers
		&stride,                // Stride (read: vertex size, or amount we move forward each vertex) for each buffer
		&offset );             // Offset into each buffer (array - we are only passing one. 
}

void RenderContext::BindIndexStream( IndexBuffer *ibo )
{
	ID3D11Buffer *handle = nullptr; 
	if (ibo != nullptr) {
		handle = ibo->m_handle; 
	}

	m_D3DContext->IASetIndexBuffer( handle, 
		DXGI_FORMAT_R32_UINT,      // 32-bit indices;            
		0 );  // byte offset 

}

void RenderContext::BindUniformBuffer( uint slot, UniformBuffer *uniformBuffer )
{
	// The API allows us to bind multiple constant buffers at once
	// and binds to each stage seperately.  For simplicity, we'll
	// just assume a slot is uniform for the entire pipeline
	ID3D11Buffer *buffer = (uniformBuffer != nullptr) ? uniformBuffer->m_handle : nullptr; 
	m_D3DContext->VSSetConstantBuffers( slot, 1U, &buffer ); 
	m_D3DContext->PSSetConstantBuffers( slot, 1U, &buffer ); 
}

void RenderContext::BeginCamera( Camera& camera )
{
	UNUSED(camera);
	if(m_currentCamera != nullptr)
	{
		ERROR_RECOVERABLE("The renderer's current camera in Begin Frame is not null. Complete pipeline with previous camera before using a new one");
		return;
	}

	//Save a reference to the current camera we are using 
	m_currentCamera = &camera;

	// first, figure out what we're rendering to; 
	ColorTargetView* colorTargetView = camera.m_colorTargetView;

	// bind targets
	ID3D11RenderTargetView *dx_rtv = nullptr; 
	uint colorCount = 0U; 
	if (colorTargetView != nullptr) {
		dx_rtv = colorTargetView->m_renderTargetView; 
		colorCount = 1U; 
	}

	// if we have a depth target, bind that as well; 
	DepthStencilTargetView *dsv = camera.m_depthStencilView; 
	ID3D11DepthStencilView *dx_dsv = nullptr; 
	if(camera.m_depthStencilView != nullptr)
	{
		if (dsv != nullptr) {
			dx_dsv = dsv->m_renderTargetView; 
		}
	}

	// Bind this as our output (this method takes an array, so 
	// this is binding an array of one)
	m_D3DContext->OMSetRenderTargets( colorCount, &dx_rtv, dx_dsv);

	// Next, we have to describe WHAT part of the texture we're rendering to (called the viewport)
	// This is also usually managed by the camera, but for now, we will just render to the whole texture
	D3D11_VIEWPORT viewport;  
	memset( &viewport, 0, sizeof(viewport) );
	viewport.TopLeftX = 0U;
	viewport.TopLeftY = 0U;
	viewport.Width = colorTargetView->m_width;
	viewport.Height = colorTargetView->m_height;
	viewport.MinDepth = 0.0f;        // must be between 0 and 1 (defualt is 0);
	viewport.MaxDepth = 1.0f;        // must be between 0 and 1 (default is 1)
	m_D3DContext->RSSetViewports( 1, &viewport );
	

	// Next, update the uniform data, and bind it as an input for the shader
	// Camera data
	m_currentCamera->UpdateUniformBuffer( this ); 
	BindUniformBuffer( UNIFORM_SLOT_CAMERA, m_currentCamera->m_cameraUBO ); 

	UpdateFrameBuffer();
	BindUniformBuffer( UNIFORM_SLOT_FRAME, m_immediateUBO);

	// Bind model matrix; 
	BindModelMatrix( Matrix44::IDENTITY ); 
	BindUniformBuffer( UNIFORM_SLOT_MODEL_MATRIX, m_modelBuffer ); 
}

void RenderContext::UpdateFrameBuffer()
{
	if(m_immediateUBO == nullptr)
	{
		m_immediateUBO = new UniformBuffer(this);
	}

	//Setup your Frame buffer struct here
	FrameBufferT frameBuffer;
	frameBuffer.frameCount = static_cast<float>(m_frameCount);
	frameBuffer.currentTime = static_cast<float>(GetCurrentTimeSeconds());
	frameBuffer.cosine = cos(frameBuffer.currentTime) * 5.f;
	frameBuffer.sine = sin(frameBuffer.currentTime) * 5.f;

	//Copy the cpu to gpu here(on your ubo)
	m_immediateUBO->CopyCPUToGPU(&frameBuffer, sizeof(frameBuffer));
}

void RenderContext::EndCamera()
{
	if(m_currentCamera == nullptr)
	{
		ERROR_RECOVERABLE("The renderer's current camera in End Frame is null");
		return;
	}

	// unbind the color targets 
	m_D3DContext->OMSetRenderTargets( 0, nullptr, nullptr ); 

	// no current camera being drawn; 
	m_currentCamera = nullptr;
}

void RenderContext::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes )
{
	DrawVertexArray(vertexes, numVertexes);
}



void RenderContext::DrawVertexArray( const std::vector<Vertex_PCU>& vertexes )
{
	if(vertexes.size() == 0)
	{
		return;
	}

	DrawVertexArray( static_cast<int>(vertexes.size()), &vertexes[0]);
}

void RenderContext::DrawVertexArray( Vertex_PCU const *vertices, uint count )
{
	if(m_immediateVBO == nullptr)
	{
		m_immediateVBO = new VertexBuffer(this);
	}

	// copy to a vertex buffer
	m_immediateVBO->CopyCPUToGPU( vertices, count);

	// bind that vertex buffer
	BindVertexStream( m_immediateVBO ); 

	Draw( count ); 
	
	//To Do implementation:
	/*
	m_immediateMesh->CopyFromVertexArray( vertices, count ); 
	m_immediateMesh->SetDrawCall( false, count ); 

	DrawMesh( m_immediateMesh ); 
	*/
}

void RenderContext::DrawMesh( GPUMesh *mesh )
{
	BindVertexStream( mesh->m_vertexBuffer ); 
	BindIndexStream( mesh->m_indexBuffer ); 

	if (mesh->UsesIndexBuffer()) {
		DrawIndexed( mesh->GetElementCount(), mesh->GetVertexCount() ); 
	} else {
		Draw( mesh->GetVertexCount() ); 
	}
}

//Depricated. To be removed
/*
TextureView* RenderContext::CreateOrGetTextureViewFromFile(std::string const &imageFilePath)
{
	std::map<std::string, TextureView*>::const_iterator requestedTexture = m_loadedTextures.find(imageFilePath);
	if(requestedTexture != m_loadedTextures.end())
	{
		//Texture requested exists in the map
		return requestedTexture->second;
	}
	else
	{
		//Create the new tex
		TextureView* texture = CreateTextureViewFromFile(imageFilePath.c_str());
		return texture;
	}
}
*/

BitmapFont* RenderContext::CreateOrGetBitmapFontFromFile(const std::string& bitmapName)
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

Shader* RenderContext::CreateOrGetShaderFromFile( std::string const &fileName )
{
	std::string filePath = SHADER_PATH + fileName;
	std::map<std::string, Shader*>::const_iterator requestedShader = m_loadedShaders.find(filePath);
	if(requestedShader != m_loadedShaders.end())
	{
		//Shader requested exists in the map
		return requestedShader->second;
	}
	else
	{
		//Create the newShader
		Shader *shader = CreateShaderFromFile(filePath); 
		return shader;
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

