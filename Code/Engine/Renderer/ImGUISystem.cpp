//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/ImGUISystem.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//Global pointer for use in Game Systems
ImGUISystem* g_ImGUI = nullptr;

//------------------------------------------------------------------------------------------------------------------------------
ImGUISystem::ImGUISystem(RenderContext* context)
{
	m_renderContext = context;
	Startup();
}

//------------------------------------------------------------------------------------------------------------------------------
ImGUISystem::~ImGUISystem()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

//------------------------------------------------------------------------------------------------------------------------------
void ImGUISystem::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
}

//------------------------------------------------------------------------------------------------------------------------------
void ImGUISystem::Render()
{
	ImGui::Render();
	ImDrawData* data = ImGui::GetDrawData();

	//Set RTV for imGUI
	ID3D11RenderTargetView *dx_rtv = m_renderContext->GetFrameColorTarget()->GetAsDXResource();
	ID3D11DeviceContext* context = m_renderContext->GetDXContext();
	context->OMSetRenderTargets(1, &dx_rtv, NULL);

	ImGui_ImplDX11_RenderDrawData(data);
}

//------------------------------------------------------------------------------------------------------------------------------
void ImGUISystem::EndFrame()
{
	
}

//------------------------------------------------------------------------------------------------------------------------------
void ImGUISystem::Startup()
{
	//Setup imGUI here
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(m_renderContext->m_hwnd);
	ImGui_ImplDX11_Init(m_renderContext->GetDXDevice(), m_renderContext->GetDXContext());
	ImGui::StyleColorsDark();
	UNUSED(io);
}

