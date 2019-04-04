#pragma once
//------------------------------------------------------------------------------------------------------------------------------
//Engine Systems
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RendererTypes.hpp"
#include <d3d11.h>

struct ID3D11SamplerState;

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

enum eFilterMode
{
	FILTER_MODE_POINT,
	FILTER_MODE_LINEAR,

	NUM_FILTER_MODES
};

class Sampler
{
public:
	Sampler();
	explicit Sampler(const std::string& samplerType);
	~Sampler();  // be sure to clean up; 

public:
	// called by the device when bound to lazily 
	// create the state object the first time, or if
	// it has changed (which is rare)
	void CreateStateIfDirty( RenderContext *ctx ); 

	inline void SetFilterModes( eFilterMode min, eFilterMode mag )    { m_minFilter = min; m_magFilter = mag; m_isDirty = true; }

	ID3D11SamplerState* GetHandle() const                             { return m_handle; }

public:
	ID3D11SamplerState *m_handle  = nullptr; 

	// state we care about
	eFilterMode m_minFilter       = FILTER_MODE_POINT; // what to do when "shrinking" an image, or a pixel on screen represents multiple texels.
	eFilterMode m_magFilter       = FILTER_MODE_POINT; // what to do when "stretching" an image, or when multiple pixels on screen fit in the same texel; 
	// eFilterMode m_mipFilter    = FILTER_MODE_POINT; // TODO when we implement mipmaps later; 

	eBlendOperation m_blendOp	  = BLEND_OP_DEFAULT;

	// there are more options, we'll introduce them as we need them
	// ...wrap
	// ...mip clamps & bias
	// ...compare samplers

	bool m_isDirty                = true; // marked when state has changed so we know to recreate; 

};
