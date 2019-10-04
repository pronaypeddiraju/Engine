#include "Engine/Commons/Profiler/ProfilerReport.hpp"
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Commons/Profiler/Profiler.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Renderer/ImGUISystem.hpp"
#include "ThirdParty/imGUI/imgui_internal.h"
#include <string.h>

ProfilerReport* gProfileReporter = nullptr;

//------------------------------------------------------------------------------------------------------------------------------
ProfilerReport::ProfilerReport()
{
	InitializeReporter();
}

//------------------------------------------------------------------------------------------------------------------------------
ProfilerReport::~ProfilerReport()
{

}

//------------------------------------------------------------------------------------------------------------------------------
ProfilerReportNode* ProfilerReport::GetFrameInHistory(uint history /*= 1*/)
{
	Profiler* profiler = gProfiler->GetInstance();

	ProfilerSample_T* sample = profiler->ProfilerAcquirePreviousTreeForCallingThread(history);

	//Traverse the tree and create duplicate tree for Reporting
	GenerateTreeFromFrame(sample);

	//Return the root as it now has the frame tree
	return m_root;
}

//------------------------------------------------------------------------------------------------------------------------------
ProfilerReportNode* ProfilerReport::GetRoot()
{
	return m_root;
}

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerReport::InitializeReporter()
{
	g_eventSystem->SubscribeEventCallBackFn("ProfilerReportFrame", Command_ProfilerReportFrame);
}

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerReport::GenerateTreeFromFrame(ProfilerSample_T* root)
{
	if (m_root != nullptr)
	{
		delete m_root;
		m_root = nullptr;
	}

	m_root = new ProfilerReportNode(root);
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC ProfilerReport* ProfilerReport::CreateInstance()
{
	if (gProfileReporter == nullptr)
	{
		gProfileReporter = new ProfilerReport();
	}

	return gProfileReporter;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC ProfilerReport* ProfilerReport::GetInstance()
{
	if (gProfileReporter == nullptr)
	{
		return CreateInstance();
	}

	return gProfileReporter;
}

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerReport::DestroyInstance()
{
	if (gProfileReporter != nullptr)
	{
		delete gProfileReporter;
		gProfileReporter = nullptr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerReport::DrawTreeViewAsImGUIWidget(uint history)
{
	//call generate for history
	ProfilerReportNode* root = GetFrameInHistory(history);

	//create and populate the imGUI widget
	ImGui::Begin("Tree View Window");
	ImGui::SetWindowPos(ImVec2(50, 350));
	ImGui::SetWindowSize(ImVec2(1650, 450));

	ImGui::Columns(5, "Tree View", true);



	ImGui::End();
}

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerReport::DrawFlatViewAsImGUIWidget(uint history)
{
	TODO("Flat View");
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool ProfilerReport::Command_ProfilerReportFrame(EventArgs& args)
{
	uint history = args.GetValue("History", 1);

	ProfilerReportNode* node = gProfileReporter->GetFrameInHistory(history);
	TODO("Handle views and send the recieved node to the required view method");

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
ProfilerReportNode::ProfilerReportNode(ProfilerSample_T* node, ProfilerReportNode* parent)
{
	//Setup all data on the ReportNode
	m_parent = parent;

	m_allocationCount = node->m_allocCount;
	m_allocationSize = node->m_allocationSizeInBytes;

	m_freeCount = node->m_freeCount;
	m_freedSize = node->m_freeSizeInBytes;

	m_numCalls = 1;

	strcpy_s(m_label, node->m_label);

	m_totalTimeHPC = node->m_endTime - node->m_startTime;
	m_totalTime = GetHPCToSeconds(m_totalTimeHPC);
	m_avgTime = m_totalTime;
	m_maxTime = m_totalTime;

	//Grab all children
	if (node->m_lastChild != nullptr)
	{
		GetChildrenFromSampleRoot(node->m_lastChild, this);
	}
	else if (node->m_prevSibling != nullptr)
	{
		GetChildrenFromSampleRoot(node->m_prevSibling, this->m_parent);
	}

	GetSelfTime();
}

//------------------------------------------------------------------------------------------------------------------------------
ProfilerReportNode::~ProfilerReportNode()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerReportNode::GetChildrenFromSampleRoot(ProfilerSample_T* root, ProfilerReportNode* parent)
{
	if (root != nullptr)
	{
		parent->m_children.emplace_back(ProfilerReportNode(root, parent));
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerReportNode::SortByTotalTime()
{
	for (int indexI = 0; indexI < m_children.size(); ++indexI)
	{
		double totalTimeI = m_children[indexI].m_totalTime;

		for (int indexJ = indexI + 1; indexJ < m_children.size(); ++indexJ)
		{
			double totalTimeJ = m_children[indexJ].m_totalTime;
			if (totalTimeI > totalTimeJ)
			{
				std::swap(m_children[indexI], m_children[indexJ]);
				totalTimeI = totalTimeJ;
			}
		}
	}

	for (ProfilerReportNode child : m_children)
	{
		(&child)->SortByTotalTime();
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerReportNode::SortBySelfTime()
{
	for (int indexI = 0; indexI < m_children.size(); ++indexI)
	{
		double selfTimeI = m_children[indexI].m_selfTime;

		for (int indexJ = indexI + 1; indexJ < m_children.size(); ++indexJ)
		{
			double selfTimeJ = m_children[indexJ].m_selfTime;
			if (selfTimeI > selfTimeJ)
			{
				std::swap(m_children[indexI], m_children[indexJ]);
				selfTimeI = selfTimeJ;
			}
		}
	}

	for (ProfilerReportNode child : m_children)
	{
		(&child)->SortBySelfTime();
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerReportNode::GetSelfTime()
{
	m_selfTime = m_totalTime;

	double childrenTime = 0;

	for (ProfilerReportNode child : m_children)
	{
		childrenTime += child.m_totalTime;
	}

	m_selfTime -= childrenTime;
}
