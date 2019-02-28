#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"

// Required Headers
#include <d3d11.h>  
#include <DXGI.h>    

// DEBUG STUFF
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

ColorTargetView::ColorTargetView()
{

}

ColorTargetView::~ColorTargetView()
{
	DX_SAFE_RELEASE(m_renderTargetView);
}

// this is an internally used method - not called outside the of the render system; 
// update the internal handle; 
void ColorTargetView::CreateForInternalTexture( ID3D11Texture2D &D3D11texture, ID3D11Device &D3Ddevice )
{
	DX_SAFE_RELEASE( m_renderTargetView ); // release the old one; 

	// next, create the rtv for it; 
	HRESULT hr = D3Ddevice.CreateRenderTargetView( &D3D11texture,  // texture to create the view for
		nullptr,   // default options (use texture info)
		&m_renderTargetView);    // out variable to store the view

	GUARANTEE_OR_DIE( SUCCEEDED(hr), "Could not create Render Target View from Texture!" ); // make sure we could get it 
									   // if this fails, check your output - DX will usually tell you why
									   // (likely texture wasn't setup to be an output)

	D3D11_TEXTURE2D_DESC textureDescription;
	D3D11texture.GetDesc(&textureDescription);
	//Set the width and the height of the target view to the m_ values 
	m_width = textureDescription.Width;
	m_height = textureDescription.Height;

}