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

	if (m_activeMode == FLAT_VIEW)
	{
		//We want flat view
		GenerateFlatFromFrame(sample);
	}
	else
	{
		//Traverse the tree and create duplicate tree for Reporting
		GenerateTreeFromFrame(sample);
	}

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
	g_eventSystem->SubscribeEventCallBackFn("ProfilerToggleMode", Command_ProfilerToggleMode);
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
void ProfilerReport::GenerateFlatFromFrame(ProfilerSample_T* root)
{
	GenerateTreeFromFrame(root);
	m_flatViewVector.clear();

	//m_flatViewVector.push_back(m_root);

	GenerateFlatViewVector(m_root);

	TODO("Make flat view vector");
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
	if (m_lastHistoryFrame == 0U)
	{
		m_lastHistoryFrame = history;
	}

	ProfilerReportNode* root = nullptr;

	//call generate for history
	if (m_root == nullptr || m_lastHistoryFrame != history)
	{
		m_lastHistoryFrame = history;
		root = GetFrameInHistory(history);
	}
	else
	{
		root = m_root;
	}

	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoInputs;
	flags |= ImGuiWindowFlags_NoMouseInputs;

	//create and populate the imGUI widget
	ImGui::Begin("Tree View Window", NULL, flags);
	ImGui::SetWindowPos(ImVec2(50, 350));
	ImGui::SetWindowSize(ImVec2(1650, 450));

	ImGui::Spacing();

	ImGui::Columns(10, "mycolumns");
	ImGui::Separator();
	ImGui::Text("Frame"); ImGui::NextColumn();
	ImGui::Text("Calls"); ImGui::NextColumn();
	ImGui::Text("Percent Total"); ImGui::NextColumn();
	ImGui::Text("Total Time"); ImGui::NextColumn();
	ImGui::Text("Percent Self"); ImGui::NextColumn();
	ImGui::Text("Self Time"); ImGui::NextColumn();
	ImGui::Text("Num Allocations"); ImGui::NextColumn();
	ImGui::Text("Allocation Size"); ImGui::NextColumn();
	ImGui::Text("Num Frees"); ImGui::NextColumn();
	ImGui::Text("Freed Size"); ImGui::NextColumn();
	ImGui::EndColumns();

	ImGui::Separator();

	/*
	ImGui::SetNextWindowContentSize(ImVec2(1600.0f, 0.0f));
	ImGui::BeginChild("##ScrollingRegion", ImVec2(1600.f, ImGui::GetFontSize() * 30), false, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::EndChild();
	*/

	PopulateTreeForImGUI(m_root);

	ImGui::End();
}

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerReport::DrawFlatViewAsImGUIWidget(uint history)
{
	if (m_lastHistoryFrame == 0U)
	{
		m_lastHistoryFrame = history;
	}

	ProfilerReportNode* root = nullptr;

	//call generate for history
	if (m_root == nullptr || m_lastHistoryFrame != history)
	{
		m_lastHistoryFrame = history;
		root = GetFrameInHistory(history);
	}
	else
	{
		root = m_root;
	}

	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoInputs;
	flags |= ImGuiWindowFlags_NoMouseInputs;

	//create and populate the imGUI widget
	ImGui::Begin("Flat View Window", NULL, flags);
	ImGui::SetWindowPos(ImVec2(50, 350));
	ImGui::SetWindowSize(ImVec2(1650, 450));

	ImGui::Spacing();

	ImGui::Columns(10, "mycolumns");
	ImGui::Separator();
	ImGui::Text("Frame"); ImGui::NextColumn();
	ImGui::Text("Calls"); ImGui::NextColumn();
	ImGui::Text("Percent Total"); ImGui::NextColumn();
	ImGui::Text("Total Time"); ImGui::NextColumn();
	ImGui::Text("Percent Self"); ImGui::NextColumn();
	ImGui::Text("Self Time"); ImGui::NextColumn();
	ImGui::Text("Num Allocations"); ImGui::NextColumn();
	ImGui::Text("Allocation Size"); ImGui::NextColumn();
	ImGui::Text("Num Frees"); ImGui::NextColumn();
	ImGui::Text("Freed Size"); ImGui::NextColumn();
	ImGui::EndColumns();

	ImGui::Separator();

	PopulateFlatForImGUI();

	ImGui::End();
}

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerReport::SetReportMode(ProfilerReportMode mode)
{
	m_activeMode = mode;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool ProfilerReport::Command_ProfilerReportFrame(EventArgs& args)
{
	uint history = args.GetValue("History", 1);

	ProfilerReportNode* node = gProfileReporter->GetFrameInHistory(history);
	TODO("Handle views and send the recieved node to the required view method");

	return true;
}

void ProfilerReport::GenerateFlatViewVector(ProfilerReportNode* root)
{
	AddToFlatViewVector(*root);

	if (root->m_children.size() != 0)
	{
		std::vector<ProfilerReportNode>::iterator nodeItr = root->m_children.begin();
		while (nodeItr != root->m_children.end())
		{
			GenerateFlatViewVector(&(*nodeItr));

			/*
			for (ProfilerReportNode node : nodeItr->m_children)
			{
				GenerateFlatViewVector(&node);
			}
			*/

			nodeItr++;
		}
	}
}

void ProfilerReport::AddToFlatViewVector(ProfilerReportNode& rootNode)
{
	if (m_flatViewVector.size() == 0)
	{
		m_flatViewVector.emplace_back(&rootNode);
		return;
	}

	std::vector<ProfilerReportNode*>::iterator itr = m_flatViewVector.begin();

	while (itr != m_flatViewVector.end())
	{
		if (strcmp((*itr)->m_label, rootNode.m_label) == 0)
		{
			//We found another one of these function calls
			(*itr)->m_numCalls++;
			(*itr)->m_totalTime += rootNode.m_totalTime;
			(*itr)->m_selfTime += rootNode.m_selfTime;
			(*itr)->m_allocationCount += rootNode.m_allocationCount;
			(*itr)->m_allocationSize += rootNode.m_allocationSize;
			(*itr)->m_freeCount += rootNode.m_freeCount;
			(*itr)->m_freedSize += rootNode.m_freedSize;

			(*itr)->m_avgTime = (*itr)->m_totalTime / (*itr)->m_numCalls;
			(*itr)->m_avgSelfTime = (*itr)->m_selfTime / (*itr)->m_numCalls;
		}
		else
		{
			//Didn't find the same function call
			m_flatViewVector.emplace_back(&rootNode);
			break;
		}

		itr++;
	}	
}

void ProfilerReport::PopulateTreeForImGUI(ProfilerReportNode* root)
{	
	ProfilerReportNode* node = root;
	ProfilerReportNode* next = nullptr;

	while (node != nullptr)
	{
		ImGui::Columns(10, "mycolumns");
		//ImGui::Text(childIterator->m_label); 

		ImGui::SetNextTreeNodeOpen(true);
		ImGui::TreeNode(node->m_label);

		ImGui::NextColumn();
		ImGui::Text(std::to_string(node->m_numCalls).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string(node->m_totalPercent).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string(node->m_totalTime).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string(node->m_selfPercent).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string(node->m_selfTime).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string(node->m_allocationCount).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string(node->m_allocationSize).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string(node->m_freeCount).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string(node->m_freedSize).c_str());

		ImGui::EndColumns();

		if (node->m_children.size() != 0)
		{
			//We have children so for each of them, enter a imGui tree node and call PopulateTree for kids
			std::vector<ProfilerReportNode>::iterator childIterator;
			childIterator = node->m_children.begin();

			while (childIterator != node->m_children.end())
			{
				next = &(*childIterator);
				PopulateTreeForImGUI(next);
				
				childIterator++;
			}
			next = nullptr;
		}
		else
		{
			next = nullptr;
		}
		ImGui::TreePop();

		node = next;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerReport::PopulateFlatForImGUI()
{
	if (m_flatViewVector.size() == 0)
	{
		ERROR_RECOVERABLE("The flat view vector is empty");
	}

	std::vector<ProfilerReportNode*>::iterator nodeItr = m_flatViewVector.begin();

	while (nodeItr != m_flatViewVector.end())
	{
		ImGui::Columns(10, "mycolumns");
		//ImGui::Text(childIterator->m_label); 

		ImGui::SetNextTreeNodeOpen(true);
		ImGui::TreeNode((*nodeItr)->m_label);

		ImGui::NextColumn();
		ImGui::Text(std::to_string((*nodeItr)->m_numCalls).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string((*nodeItr)->m_totalPercent).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string((*nodeItr)->m_totalTime).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string((*nodeItr)->m_selfPercent).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string((*nodeItr)->m_selfTime).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string((*nodeItr)->m_allocationCount).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string((*nodeItr)->m_allocationSize).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string((*nodeItr)->m_freeCount).c_str()); ImGui::NextColumn();
		ImGui::Text(std::to_string((*nodeItr)->m_freedSize).c_str());

		ImGui::EndColumns();
		ImGui::TreePop();

		nodeItr++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
bool ProfilerReport::Command_ProfilerToggleMode(EventArgs& args)
{
	UNUSED(args);

	if (gProfileReporter->m_activeMode == TREE_VIEW)
	{
		gProfileReporter->m_activeMode = FLAT_VIEW;
	}
	else
	{
		gProfileReporter->m_activeMode = TREE_VIEW;
	}

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

//------------------------------------------------------------------------------------------------------------------------------
bool ProfilerReportNode::operator==(const ProfilerReportNode& compare) const
{
	if (strcmp(m_label, compare.m_label))
	{
		//We are equal if we have the same label
		return true;
	}
	else
	{
		return false;
	}
}
