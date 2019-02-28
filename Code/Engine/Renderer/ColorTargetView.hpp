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
	uint					m_width = 0U;
	uint					m_height = 0U;

private:
	void			CreateForInternalTexture( ID3D11Texture2D &tex, ID3D11Device &D3Ddevice );
};