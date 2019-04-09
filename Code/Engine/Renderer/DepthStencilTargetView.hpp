//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/IntVec2.hpp"

//------------------------------------------------------------------------------------------------------------------------------
struct ID3D11Resource;
struct ID3D11DepthStencilView;
class RenderContext;

//------------------------------------------------------------------------------------------------------------------------------
class DepthStencilTargetView                             // A04
{

public: 
	DepthStencilTargetView();                          // A04
	~DepthStencilTargetView();                         // A04

	float GetWidth() const;                            // A04
	float GetHeight() const;                           // A04

	void ClearDepthStencilView(RenderContext* renderContext, float depth);

public:
	
	ID3D11Resource*						m_source				= nullptr;        // A04
	ID3D11DepthStencilView*				m_renderTargetView		= nullptr;        // A04
	IntVec2								m_dimensions			= IntVec2::ZERO;     // A04
}; 