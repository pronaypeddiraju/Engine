//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "ThirdParty/imGUI/imgui.h"
#include "ThirdParty/imGUI/imgui_impl_dx11.h"
#include "ThirdParty/imGUI/imgui_impl_win32.h"

class RenderContext;

//------------------------------------------------------------------------------------------------------------------------------
class ImGUISystem
{
public:
	ImGUISystem(RenderContext* context);
	~ImGUISystem();

	void		BeginFrame();
	void		Render();
	void		EndFrame();

private:
	void		Startup();

public:
	RenderContext* m_renderContext = nullptr;
};