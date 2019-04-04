//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"

struct ID3D11ShaderResourceView;
struct ID3D11Resource;
class Sampler;

//------------------------------------------------------------------------------------------------------------------------------
class TextureView
{
public:
	TextureView(); 
	virtual ~TextureView(); // be sure to release your D3D11 resources (view and source)

	void SetSampler( Sampler *samp )       { m_sampler = samp; }

public:
	// things this holds onto
	ID3D11ShaderResourceView *m_view = nullptr;  // D3D11 view we bind
	ID3D11Resource *m_source         = nullptr;  // resource this is a view of 

	// things we're just referencing; 
	Sampler *m_sampler               = nullptr;  // optional sampler to use; 

	// TODO: FAR FUTURE!
	// this is a good use case for ref-counting (since each view should
	// hold onto the texture it is a view of.  For now, we'll just
	// taking advantage of D3D11's ref counting, but in other APIs (D3D12 or Vulkan)
	// you would need to do this manually to keep the texture from disappearing from under the view
	// std::shared_ptr<Texture> m_source_texture; 
}; 

// Specializing with extra information for 2D views
// Created FROM a texture; 
class TextureView2D : public TextureView 
{
public:
	IntVec2							m_size = IntVec2::ZERO; // width and height
}; 
