//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/ImGUISystem.hpp"
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

}

//------------------------------------------------------------------------------------------------------------------------------
void ImGUISystem::Render()
{
	
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
	ImGui_ImplDX11_Init(m_renderContext->m_D3DDevice, m_renderContext->m_D3DContext);
	ImGui::StyleColorsDark();
}

