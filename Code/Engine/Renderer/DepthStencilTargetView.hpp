//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec2.hpp"

struct ID3D11Resource;
struct ID3D11DepthStencilView;

class DepthStencilTargetView                             // A04
{

public: 
	DepthStencilTargetView();                          // A04
	~DepthStencilTargetView();                         // A04

	float GetWidth() const;                            // A04
	float GetHeight() const;                           // A04

public:
	
	ID3D11Resource*						m_source				= nullptr;        // A04
	ID3D11DepthStencilView*				m_renderTargetView		= nullptr;        // A04
	Vec2								m_size					= Vec2::ZERO;     // A04

}; 