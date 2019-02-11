//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"

struct D3D11_SAMPLER_DESC;

Sampler::Sampler()
{

}

Sampler::~Sampler()
{

}

STATIC D3D11_FILTER Sampler::DXGetFilter( eFilterMode min, eFilterMode mag ) 
{
	if (min == FILTER_MODE_POINT) 
	{
		switch (mag) 
		{
		case FILTER_MODE_POINT:  
			return D3D11_FILTER_MIN_MAG_MIP_POINT; 

		case FILTER_MODE_LINEAR: 
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT; 
		}; 
	} 
	else 
	{
		switch (mag) 
		{
		case FILTER_MODE_POINT:  
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT; 
		case FILTER_MODE_LINEAR: 
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; 
		};
	}

	// this combination was not handled?  
	ASSERT_RETURN_VALUE(false, D3D11_FILTER_MIN_MAG_MIP_POINT); 
}

void Sampler::CreateStateIfDirty( RenderContext *renderContext )
{
	// no changes needed
	if (false == m_isDirty) {
		return; 
	}

	// Release old state; 
	DX_SAFE_RELEASE( m_handle ); 

	// Create new state
	D3D11_SAMPLER_DESC desc;
	memset( &desc, 0, sizeof(desc)); 

	desc.Filter = DXGetFilter( m_minFilter, m_magFilter );
	desc.MaxAnisotropy = 1U; // anistropic filtering (we're not using this... yet)

							 // set texture to wrap on UV, but clamp on W (mip).  
							 // (if you're smapling 3D wrapping noise, you may want wrap in all dimensions)
							 // (if you're doing single sprites, you may want clamp all so you don't get edge bleeding)
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;  
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; 
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	// set tha tcan can sample from all mip levels; 
	desc.MinLOD = -FLT_MAX; 
	desc.MaxLOD = FLT_MAX;   
	desc.MipLODBias = 0.0f;  // will shift the mip level for a sample by this

	desc.ComparisonFunc = D3D11_COMPARISON_NEVER; // will come into play doing shadow maps; 

												   // leaving border color black (only useful if our wrap mode is BORDER

	// Create!
	renderContext->Create( &desc, &m_handle ); 

	m_isDirty = false; 
	ASSERT( m_handle != nullptr );   // this should honestly never fail.  If it does - check your output!
}; 