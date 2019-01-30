#pragma once
#include "Engine/Commons/EngineCommon.hpp"

class RenderContext;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11Device;

class ColorTargetView
{
	friend class RenderContext;

public:

	ColorTargetView();
	~ColorTargetView();

	//float			GetWidth() const;
	//float			GetHeight() const;

public:
	//Renderable view of a texture
	ID3D11RenderTargetView	*m_renderTargetView = nullptr;
	float					m_width = 0.0f;
	float					m_height = 0.0f;

private:
	void			CreateForInternalTexture( ID3D11Texture2D &tex, ID3D11Device &D3Ddevice );
};