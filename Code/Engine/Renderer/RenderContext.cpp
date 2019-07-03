#include "Engine/Renderer/RenderContext.hpp"
//3rd Party tools
#include "ThirdParty/stb/stb_image.h"
//Core systems
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/WindowContext.hpp"
//Math
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
//Rendering systems
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Renderer/DepthStencilTargetView.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Model.hpp"
#include "Engine/Renderer/ObjectLoader.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places

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
		//debugObject->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
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

//------------------------------------------------------------------------------------------------------------------------------
RenderContext::RenderContext(WindowContext* window)
{
	m_hwnd = window->m_hwnd;
	D3D11Setup(window->m_hwnd);
	Startup();
}

//------------------------------------------------------------------------------------------------------------------------------
RenderContext::~RenderContext()
{
	Shutdown();
}

//------------------------------------------------------------------------------------------------------------------------------
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

	m_cpuLightBuffer = LightBufferT();

	m_FXCam = new Camera();
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::PremakeDefaults()
{
	// premake defaults 
	std::string name = "WHITE.png";
	CreateOrGetTextureViewFromFile(name);
	
	name = "FLAT.png";
	CreateOrGetTextureViewFromFile(name);

	name = "BLACK.png";
	CreateOrGetTextureViewFromFile(name);

}

//------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::SetDepth( bool write )
{
	m_currentShader->m_writeDepth = write;
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::BindMaterial( Material* material )
{
	//Bind the shader
	BindShader(material->m_shader);

	//Bind the texture views
	int numTexs = material->GetNumTextures();
	if (numTexs == 0)
	{
		BindTextureView(0U, nullptr);
	}
	else
	{
		for (int index = 0; index < numTexs; index++)
		{
			BindTextureView(index, material->GetTextureView(index));
		}
	}

	//Bind the samplers
	int numSamplers = material->GetNumSamplers();
	for(int index= 0; index < numSamplers; index++)
	{
		BindSampler(index, material->GetSampler(index));
	}

	//bind user(material constant) buffer if available
	BindUniformBuffer(8U, material->m_materialBuffer);
}

//------------------------------------------------------------------------------------------------------------------------------
Sampler* RenderContext::GetSamplerOfType( const std::string& m_samplerType )
{
	if(m_samplerType == "linear")
	{
		return m_cachedSamplers[SAMPLE_MODE_LINEAR];
	}
	else if (m_samplerType == "point")
	{
		return m_cachedSamplers[SAMPLE_MODE_POINT];
	}

	GUARANTEE_RECOVERABLE(true, "The sampler of type passed was not found on RenderContext");
	return nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
BitmapFont* RenderContext::CreateBitmapFontFromFile(const std::string& bitmapName)
{
	//std::string filePath = "Data/Fonts/" + bitmapName + ".png";
	TextureView* bitmapTexture = CreateOrGetTextureViewFromFile(bitmapName.c_str(), true);

	BitmapFont* newFont = new BitmapFont(bitmapName, bitmapTexture);

	m_loadedFonts[bitmapName] = newFont;
	return newFont;
}

//------------------------------------------------------------------------------------------------------------------------------
Shader* RenderContext::CreateShaderFromFile(const std::string& fileName)
{
	//Check the file extention
	std::vector<std::string> strings = SplitStringOnDelimiter(fileName, '.');
	bool isDataDriven = false;
	if(strings.size() > 1)
	{
		for(int i = 0; i < (int)strings.size(); i++)
		{
			if(strings[i] == "xml")
			{
				isDataDriven = true;
			}
		}
	}

	Shader* shader = new Shader();
	char* outData = nullptr;
	unsigned long bufferSize;
	if(isDataDriven)
	{
		//Load the Shader from XML
		shader->LoadShaderFromXMLSource(fileName);

		bufferSize = CreateFileBuffer( shader->m_shaderSourcePath, &outData); 

		shader->m_vertexStage.LoadShaderFromSource(this, shader->m_shaderSourcePath, outData, bufferSize, SHADER_STAGE_VERTEX);
		shader->m_pixelStage.LoadShaderFromSource(this, shader->m_shaderSourcePath, outData, bufferSize, SHADER_STAGE_FRAGMENT);
	}
	else
	{
		bufferSize = CreateFileBuffer( fileName, &outData); 

		shader->m_vertexStage.LoadShaderFromSource(this, fileName, outData, bufferSize, SHADER_STAGE_VERTEX );
		shader->m_pixelStage.LoadShaderFromSource(this, fileName, outData, bufferSize, SHADER_STAGE_FRAGMENT);
	}

	//Delete your outData!
	delete[] outData;

	m_loadedShaders[fileName] = shader;
	return shader;
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::UpdateLightBuffer()
{
	if (m_gpuLightBuffer == nullptr) 
	{
		m_gpuLightBuffer = new UniformBuffer( this ); 
	}

	// TODO: This method is called during BeginCamera - so we know our outputs
	// are final - so if you have enough data to figure out an aspect correct
	// projection, you can do it here;  For now, we'll keep with the SD1
	// version that just sets a fixed ortho; 

	if (m_lightBufferDirty) 
	{
		m_lightBufferDirty = false; 
		m_gpuLightBuffer->CopyCPUToGPU( &m_cpuLightBuffer, sizeof(m_cpuLightBuffer)); 
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::EnableDirectionalLight()
{
	m_cpuLightBuffer.lights[0].color.a = 1.f;
	m_lightBufferDirty = true;
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::EnableDirectionalLight(const Vec3& position, const Vec3& direction, const Rgba& color /*= Rgba::WHITE*/, float intensity /*= 1.f*/, const Vec3& diffuseAttenuation /*= Vec3(1.f, 0.f, 0.f)*/, const Vec3& specularAttenuation /*= Vec3(1.f, 0.f, 0.f)*/) const
{
	LightT directionalLight;

	directionalLight.position = position;
	directionalLight.color = color;
	directionalLight.color.a = intensity;
	directionalLight.direction = direction;
	directionalLight.isDirection = 1.f;
	directionalLight.diffuseAttenuation = diffuseAttenuation;
	directionalLight.specularAttenuation = specularAttenuation;

	g_renderContext->EnableLight(0U, directionalLight);
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::DisableDirectionalLight()
{
	m_cpuLightBuffer.lights[0].color.a = 0.f;
	m_lightBufferDirty = true;
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::EnablePointLight(uint slot, const Vec3& position, const Vec3& direction, const Rgba& color /*= Rgba::WHITE*/, float intensity /*= 1.f*/, const Vec3& diffuseAttenuation /*= Vec3(1.f, 0.f, 0.f)*/, const Vec3& specularAttenuation /*= Vec3(1.f, 0.f, 0.f)*/) const
{
	LightT pointLight;

	pointLight.position = position;
	pointLight.color = color;
	pointLight.color.a = intensity;
	pointLight.direction = direction;
	pointLight.diffuseAttenuation = diffuseAttenuation;
	pointLight.specularAttenuation = specularAttenuation;

	g_renderContext->EnableLight(slot, pointLight);
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::CreateAndSetDefaultRasterState()
{
	DX_SAFE_RELEASE(m_defaultRasterState);

	D3D11_RASTERIZER_DESC desc; 

	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = TRUE; 
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

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::SetRasterStateWireFrame()
{
	DX_SAFE_RELEASE(m_defaultRasterState);

	D3D11_RASTERIZER_DESC desc; 

	desc.FillMode = D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = TRUE; 
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

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::BeginFrame()
{
	// Get the back buffer
	ID3D11Texture2D *back_buffer = nullptr;
	m_D3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);

	// Okay, we have an rtv, store it in our own class
	m_FrameBuffer_ColorTargetView = new ColorTargetView();
	m_FrameBuffer_ColorTargetView->CreateForInternalTexture( *back_buffer, *m_D3DDevice ); 	

	m_defaultDepthTexture = Texture2D::CreateDepthStencilTarget(this, m_FrameBuffer_ColorTargetView->m_width, m_FrameBuffer_ColorTargetView->m_height);
	m_defaultDepthStencilView = m_defaultDepthTexture->CreateDepthStencilTargetView();
	
	m_defaultColorTexture = Texture2D::CreateColorTarget(this, m_FrameBuffer_ColorTargetView->m_width, m_FrameBuffer_ColorTargetView->m_height);
	m_defaultColorTargetView = new ColorTargetView();
	m_defaultColorTargetView->CreateForInternalTexture(*(ID3D11Texture2D*)m_defaultColorTexture->m_handle, *m_D3DDevice);

	m_FXTexture = Texture2D::CreateColorTarget(this, m_FrameBuffer_ColorTargetView->m_width, m_FrameBuffer_ColorTargetView->m_height);
	m_FXColorTargetView = new ColorTargetView();
	m_FXColorTargetView->CreateForInternalTexture(*(ID3D11Texture2D*)m_FXTexture->m_handle, *m_D3DDevice);

	m_defaultDepthStencilView->ClearDepthStencilView(this, 1.0f);

	delete m_defaultDepthTexture;
	m_defaultDepthTexture = nullptr;

	//ColorTargetView holds a reference to the back_buffer so we can now release it from this function
	DX_SAFE_RELEASE( back_buffer );

}

//------------------------------------------------------------------------------------------------------------------------------
ColorTargetView* RenderContext::GetFrameColorTarget()
{
	return m_defaultColorTargetView;
}

//------------------------------------------------------------------------------------------------------------------------------
Texture2D* RenderContext::GetFrameColorTexture()
{
	return m_defaultColorTexture;
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::EndFrame()
{
	Texture2D* backBufferTexture = new Texture2D(this);
	
	// Get the back buffer
	ID3D11Texture2D *back_buffer = nullptr;
	m_D3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);

	backBufferTexture->m_handle = back_buffer;

	CopyTexture(backBufferTexture, m_defaultColorTexture);

	// We're done rendering, so tell the swap chain they can copy the back buffer to the front (desktop/window) buffer
	m_D3DSwapChain->Present( 0, // Sync Interval, set to 1 for VSync
		0 );                    // Present flags, see;
								// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509554(v=vs.85).aspx

	//Free up the color target view or we have a leak in memory 
	delete m_FrameBuffer_ColorTargetView;
	m_FrameBuffer_ColorTargetView = nullptr;

	delete m_defaultDepthStencilView;
	m_defaultDepthStencilView = nullptr;

	delete m_defaultColorTexture;
	m_defaultColorTexture = nullptr;

	delete m_defaultColorTargetView;
	m_defaultColorTargetView = nullptr;

	delete m_FXTexture;
	m_FXTexture = nullptr;

	delete m_FXColorTargetView;
	m_FXColorTargetView = nullptr;

	delete backBufferTexture;
	backBufferTexture = nullptr;

	// We don't need to release back buffer anymore as it is being released when we delete the backBufferTexture as the texture stores it as a handle
	//DX_SAFE_RELEASE(back_buffer);
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::Shutdown()
{
	delete m_FXCam;
	m_FXCam = nullptr;

	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	delete m_immediateUBO;
	m_immediateUBO = nullptr;

	delete m_gpuLightBuffer;
	m_gpuLightBuffer = nullptr;

	delete m_FrameBuffer_ColorTargetView;
	m_FrameBuffer_ColorTargetView = nullptr;

	delete m_defaultDepthStencilView;
	m_defaultDepthStencilView = nullptr;

	delete m_defaultColorTargetView;
	m_defaultColorTargetView = nullptr;

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

	//Clear all Fonts
	std::map< std::string, BitmapFont*>::iterator fontIterator;
	std::map< std::string, BitmapFont*>::iterator lastFontIterator;

	fontIterator = m_loadedFonts.begin();
	lastFontIterator = m_loadedFonts.end();

	for(fontIterator; fontIterator != lastFontIterator; fontIterator++)
	{
		delete fontIterator->second;
	}

	m_loadedFonts.clear();

	//Clear all Materials
	std::map< std::string, Material*>::iterator MatIterator;
	std::map< std::string, Material*>::iterator lastMatIterator;

	MatIterator = m_materialDatabase.begin();
	lastMatIterator = m_materialDatabase.end();

	for(MatIterator; MatIterator != lastMatIterator; MatIterator++)
	{
		delete MatIterator->second;
	}

	m_materialDatabase.clear();

	//Clear all Models
	std::map< std::string, GPUMesh*>::iterator modelIterator;
	std::map< std::string, GPUMesh*>::iterator lastModelIterator;

	modelIterator = m_modelDatabase.begin();
	lastModelIterator = m_modelDatabase.end();

	for (modelIterator; modelIterator != lastModelIterator; modelIterator++)
	{
		delete modelIterator->second;
	}

	m_modelDatabase.clear();

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

//------------------------------------------------------------------------------------------------------------------------------
ID3D11DeviceContext* RenderContext::GetDXContext() const
{
	return m_D3DContext;
}

//------------------------------------------------------------------------------------------------------------------------------
ID3D11Device* RenderContext::GetDXDevice() const
{
	return m_D3DDevice;
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::SetAmbientLight( Rgba const &color, float intensity )
{
	m_cpuLightBuffer.ambient = color;
	m_cpuLightBuffer.ambient.a = intensity;
	m_lightBufferDirty = true;
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::EnableLight( uint slot, LightT const &info )
{
	m_cpuLightBuffer.lights[slot] = info;
	m_lightBufferDirty = true;
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::DisableLight( uint slot )
{
	m_cpuLightBuffer.lights[slot] = LightT();
	m_cpuLightBuffer.ambient.a = 0.f;
	m_lightBufferDirty = true;
}

//------------------------------------------------------------------------------------------------------------------------------
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

	shader->UpdateDepthStateIfDirty(this);
	m_D3DContext->OMSetDepthStencilState(shader->m_depthStencilState, 1U);

	m_currentShader = shader;
}

//------------------------------------------------------------------------------------------------------------------------------
// Note:  We have 128 texture slots, and 16 sampler slots
// But for simplicity, the system is designed to have the sampler stored
// with the texture view (so users just have to say "BindTexture").  
// Meaning, this design limits us to 16 textures.  
//------------------------------------------------------------------------------------------------------------------------------
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
			BindTextureView(0U, "WHITE.png");
		}
	}

	// You can bind textures to the Vertex stage, but not samplers
	// We're *not* for now since no effect we do at Guildhall requires it, but
	// be aware it is an option; 
	// m_context->VSSetShaderResource( slot, 1U, &srv ); 
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::BindTextureView( uint slot, std::string const &name )
{
	std::map<std::string, TextureView*>::iterator textureIterator;
	textureIterator = m_cachedTextureViews.find(name);

	BindTextureView(slot, textureIterator->second);
}

//------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::BindTextureViewWithSampler( uint slot, TextureView *view )
{
	BindTextureView( slot, view ); 

	if (view != nullptr) {
		BindSampler( slot, view->m_sampler );
	} else {
		BindSampler( slot, nullptr ); 
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::BindTextureViewWithSampler( uint slot, std::string const &name )
{
	std::map< std::string, TextureView* >::iterator textureIterator;
	textureIterator = m_cachedTextureViews.find(name);

	if(textureIterator == m_cachedTextureViews.end())
	{
		//Only for now. Later add a line of code that calls GetOrCreateTextureView
		ERROR_AND_DIE("Texture requested does not exist");
	}

	BindTextureViewWithSampler(slot, textureIterator->second);
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::BindTextureViewWithSampler( uint slot, TextureView *view, Sampler *sampler )
{
	view->m_sampler = sampler;
	BindTextureViewWithSampler(slot, view);
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::BindTextureViewWithSampler( uint slot, TextureView *view, eSampleMode mode )
{
	view->m_sampler = m_cachedSamplers[mode];
	BindTextureViewWithSampler(slot, view);
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::BindTextureViewWithSampler( uint slot, std::string const &name, eSampleMode mode )
{
	std::map<std::string, TextureView*>::iterator texIterator;
	texIterator = m_cachedTextureViews.find(name);

	BindTextureViewWithSampler(slot, texIterator->second, mode);
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::BindModelMatrix( Matrix44 const &model ) 
{
	m_cpuModelBuffer.ModelMatrix = model; 

	m_modelBuffer->CopyCPUToGPU( &m_cpuModelBuffer, sizeof(m_cpuModelBuffer) ); 
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::SetGlobalTint( const Rgba& color )
{
	m_cpuModelBuffer.TintColor = color;

	m_modelBuffer->CopyCPUToGPU( &m_cpuModelBuffer, sizeof(m_cpuModelBuffer) ); 
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::SetModelMatrix( Matrix44 const &modelMatrix )
{
	BindModelMatrix(modelMatrix);
}

//------------------------------------------------------------------------------------------------------------------------------
// (NOTE: This design is fairly different from my Engine, 
// so while I'm fairly sure this should work, if it doesn't, please let me know)
//------------------------------------------------------------------------------------------------------------------------------
TextureView* RenderContext::CreateOrGetTextureViewFromFile( std::string const &filename, bool isFont )
{
	TextureView* view = nullptr; 

	// normal stuff - if it exists, return it; 
	std::map<std::string, TextureView*>::iterator item = m_cachedTextureViews.find(filename); 
	if (item != m_cachedTextureViews.end()) 
	{
		return item->second; 
	} 

	Texture2D *tex = new Texture2D(this); 
	DebuggerPrintf( "Loading: %s\n", filename.c_str() ); 

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

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::RegisterTextureView(std::string const &fileName, TextureView const *view)
{ 
	std::map<std::string, TextureView*>::iterator item = m_cachedTextureViews.find(fileName);
	if (item != m_cachedTextureViews.end())
	{
		return;
	}
	else
	{
		m_cachedTextureViews[fileName] = (TextureView*)view;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::Draw( uint vertexCount, uint byteOffset )
{
	// **NEW** - before a draw can happen, 
	// We need to describe the input to the shader
	// TODO: only create an input layout if the vertex type changes; 
	// TODO: When different vertex types come on-line, look at the current bound
	//       input streams (VertexBuffer) for the layout
	
	// TODO: m_currentShader->CreateInputLayoutFor( VertexPCU::LAYOUT ); 

	m_D3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_D3DContext->IASetInputLayout( m_currentShader->m_inputLayout );
	m_D3DContext->Draw( vertexCount, byteOffset ); 	

}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::DrawIndexed( uint indexCount)
{
	//bool result =  m_currentShader->CreateInputLayoutForVertexPCU(); 

	m_D3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_D3DContext->IASetInputLayout( m_currentShader->m_inputLayout );

	// Draw
	m_D3DContext->DrawIndexed( indexCount, 
		0,       // elem offset 
		0 );     // vert offset 
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::ClearColorTargets( const Rgba& clearColor )
{
	// Clear the buffer.
	float clearColorArray[4] = { clearColor.r, clearColor.g, clearColor.b, clearColor.a};
	m_D3DContext->ClearRenderTargetView( m_currentCamera->m_colorTargetView->m_renderTargetView, clearColorArray);
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::ClearDepthStencilTarget( float depth /*= 1.0f*/, uint8_t stencil /*= 0U */ )
{
	ID3D11DepthStencilView *dsv = nullptr; 
	dsv = m_currentCamera->m_depthStencilView->m_renderTargetView; 
	m_D3DContext->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil ); 
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::BindVertexStream( VertexBuffer *vbo )
{
	// Bind the input stream; 
	uint stride = (uint)vbo->m_elementSize;
	uint offset = 0U;
	m_D3DContext->IASetVertexBuffers( 0,    // Start slot index
		1,                            // Number of buffers we're binding
		&vbo->m_handle, // Array of buffers
		&stride,                // Stride (read: vertex size, or amount we move forward each vertex) for each buffer
		&offset );             // Offset into each buffer (array - we are only passing one. 
}

//------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::BindUniformBuffer( uint slot, UniformBuffer *uniformBuffer )
{
	// The API allows us to bind multiple constant buffers at once
	// and binds to each stage seperately.  For simplicity, we'll
	// just assume a slot is uniform for the entire pipeline
	ID3D11Buffer *buffer = (uniformBuffer != nullptr) ? uniformBuffer->m_handle : nullptr; 
	m_D3DContext->VSSetConstantBuffers( slot, 1U, &buffer ); 
	m_D3DContext->PSSetConstantBuffers( slot, 1U, &buffer ); 
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::CopyTexture(Texture2D *dst, Texture2D *src)
{
	m_D3DContext->CopyResource((ID3D11Resource*)dst->m_handle, (ID3D11Resource*)src->m_handle);
}

//------------------------------------------------------------------------------------------------------------------------------
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
	if (dsv != nullptr) 
	{
		dx_dsv = dsv->m_renderTargetView; 
	}
	else
	{
		dx_dsv = m_defaultDepthStencilView->m_renderTargetView;
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
	viewport.Width = static_cast<float>(colorTargetView->m_width);
	viewport.Height = static_cast<float>(colorTargetView->m_height);
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

//------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::UpdateFxBuffer(float effectStrenght)
{
	if (m_immediateUBO == nullptr)
	{
		m_immediateUBO = new UniformBuffer(this);
	}

	Vec4 FXIntensity(effectStrenght, effectStrenght, effectStrenght, effectStrenght);
	//Copy the cpu to gpu here(on your ubo)
	m_immediateUBO->CopyCPUToGPU(&FXIntensity, sizeof(FXIntensity));
}

//------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes )
{
	DrawVertexArray(vertexes, numVertexes);
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::DrawVertexArray( const std::vector<Vertex_PCU>& vertexes )
{
	if(vertexes.size() == 0)
	{
		return;
	}

	DrawVertexArray( static_cast<int>(vertexes.size()), &vertexes[0]);
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::DrawVertexArray( Vertex_PCU const *vertices, uint count )
{
	if(m_immediateVBO == nullptr)
	{
		m_immediateVBO = new VertexBuffer(this);
	}

	// copy to a vertex buffer
	m_immediateVBO->CopyCPUToGPU( vertices, count, sizeof(Vertex_PCU));

	// bind that vertex buffer
	BindVertexStream( m_immediateVBO ); 
	bool result = m_currentShader->CreateInputLayout(Vertex_PCU::layout);
	if(result)
	{
		Draw( count ); 
	}
	else
	{
		ERROR_AND_DIE("Could not create Shader Input Layout for Vertex_PCU");
	}
	
	//To Do implementation:
	/*
	m_immediateMesh->CopyFromVertexArray( vertices, count ); 
	m_immediateMesh->SetDrawCall( false, count ); 

	DrawMesh( m_immediateMesh ); 
	*/
}

//------------------------------------------------------------------------------------------------------------------------------
bool RenderContext::PreDraw( GPUMesh *mesh)
{
	//Bind the uniforms
	UpdateLightBuffer();
	BindUniformBuffer(UNIFORM_SLOT_LIGHT, m_gpuLightBuffer);

	//Bind vertex and index streams
	BindVertexStream( mesh->m_vertexBuffer ); 
	BindIndexStream( mesh->m_indexBuffer ); 

	//Creat the input layout based on the mesh's layout
	bool result = m_currentShader->CreateInputLayout(mesh->m_layout);
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::DrawMesh( GPUMesh *mesh )
{
	bool result = PreDraw(mesh);

	if(result)
	{
		if (mesh->UsesIndexBuffer()) 
		{
			DrawIndexed( mesh->GetElementCount()); 
		} 
		else 
		{
			Draw( mesh->GetVertexCount() ); 
		}
	}
	else
	{
		ERROR_AND_DIE("Could not create input layout!");
	}
}

//------------------------------------------------------------------------------------------------------------------------------
// Version to apply the effect on the default FX Color Target
//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::ApplyEffect(Material *mat)
{
	m_FXCam->SetOrthoView(Vec2::ZERO, Vec2((float)m_defaultColorTargetView->m_width, (float)m_defaultColorTargetView->m_height));
	m_FXCam->SetColorTarget(m_FXColorTargetView);

	BeginCamera(*m_FXCam);

	TextureView* tex = m_defaultColorTexture->CreateTextureView();
	mat->SetTextureView(0, tex);
	BindMaterial(mat);

	Draw(3);

	EndCamera();
	CopyTexture(m_defaultColorTexture, m_FXTexture);

	delete tex;
	tex = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
void RenderContext::ApplyEffect(Texture2D *dst, Texture2D *src, Material *mat)
{
	ColorTargetView* sourceTarget = src->CreateColorTargetView();
	ColorTargetView* destTarget = dst->CreateColorTargetView();

	m_FXCam->SetOrthoView(Vec2::ZERO, Vec2((float)sourceTarget->m_width, (float)sourceTarget->m_height));
	m_FXCam->SetColorTarget(destTarget);

	BeginCamera(*m_FXCam);

	TextureView* tex = src->CreateTextureView();
	mat->SetTextureView(0, tex);
	BindMaterial(mat);

	Draw(3);

	EndCamera();

	Texture2D* backBufferTexture = new Texture2D(this);

	// Get the back buffer
	ID3D11Texture2D *back_buffer = nullptr;
	m_D3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);

	backBufferTexture->m_handle = back_buffer;

	CopyTexture(dst, m_FXTexture);

	delete tex;
	tex = nullptr;

	delete backBufferTexture;
	backBufferTexture = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
IntVec2 RenderContext::GetCurrentScreenDimensions()
{
	return g_windowContext->GetClientBounds();
}

//------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------
Material* RenderContext::CreateOrGetMaterialFromFile( const std::string& fileName )
{
	std::vector<std::string> splits = SplitStringOnDelimiter(fileName, '/');
	bool isModel = false;
	if ((int)splits.size() > 1)
	{
		isModel = true;
	}

	std::string filePath;
	if (!isModel)
	{
		filePath = MATERIAL_PATH + fileName;
	}
	else
	{
		filePath = fileName;
	}

	std::map<std::string, Material*>::const_iterator requestedMaterial = m_materialDatabase.find(filePath);
	if(requestedMaterial != m_materialDatabase.end())
	{
		//Material requested exists in the map
		return requestedMaterial->second;
	}
	else
	{
		//Create the Material
		Material *material = new Material(this, filePath); 
		return material;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
GPUMesh* RenderContext::CreateOrGetMeshFromFile(const std::string& fileName)
{
	std::string filePath = MODEL_PATH + fileName;
	std::map<std::string, GPUMesh*>::const_iterator requestedModel = m_modelDatabase.find(filePath);
	if (requestedModel != m_modelDatabase.end())
	{
		//Model requested exists in the map
		return requestedModel->second;
	}
	else
	{
		//Check the file extention
		std::vector<std::string> strings = SplitStringOnDelimiter(fileName, '.');
		bool isDataDriven = false;
		if (strings.size() > 1)
		{
			if (strings[(strings.size() - 1)] == "mesh")
			{
				isDataDriven = true;
			}
		}

		//Create the Model
		ObjectLoader* model = ObjectLoader::CreateMeshFromFile(this, filePath, isDataDriven);
		
		//Setup materials
		std::vector<std::string> splits = SplitStringOnDelimiter(fileName, '.');
		if (splits[splits.size() - 1] == "obj" || splits[splits.size() - 1] == "mesh")
		{
			filePath = MODEL_PATH + splits[0] + ".mat";
			model->m_mesh->m_defaultMaterial = filePath;
		}

		filePath = MODEL_PATH + fileName;
		m_modelDatabase[filePath] = model->m_mesh;
		return m_modelDatabase[filePath];
	}
}

