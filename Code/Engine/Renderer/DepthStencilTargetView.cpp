//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/DepthStencilTargetView.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"

// Required Headers
#include <d3d11.h>  
#include <DXGI.h>    

// DEBUG STUFF
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

DepthStencilTargetView::DepthStencilTargetView()
{

}

DepthStencilTargetView::~DepthStencilTargetView()
{
	DX_SAFE_RELEASE(m_renderTargetView);
	DX_SAFE_RELEASE(m_source);
}

void DepthStencilTargetView::ClearDepthStencilView( RenderContext* renderContext, float depth )
{
	renderContext->m_D3DContext->ClearDepthStencilView(m_renderTargetView, 1U, depth, 0U);
}

