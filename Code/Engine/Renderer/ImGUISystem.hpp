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

	int			CurveEditor(const char* label, float* values, int points_count, const ImVec2& editor_size, int* new_count);
private:
	void		Startup();

public:
	RenderContext* m_renderContext = nullptr;
};