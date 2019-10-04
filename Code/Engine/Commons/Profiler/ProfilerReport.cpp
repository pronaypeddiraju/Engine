#include "Engine/Commons/Profiler/ProfilerReport.hpp"
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Commons/Profiler/Profiler.hpp"
#include "Engine/Core/NamedProperties.hpp"
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

	/*
	ProfilerSample_T* node = nullptr;
	ProfilerSample_T* next = nullptr;

	node = root;
	while (node->m_refCount != 0)
	{
		if (node->m_lastChild->m_refCount != 0)
		{
			next = node->m_lastChild;
			m_root MakeNode(node->m_lastChild);
		}
		else
		{
			if (node->m_prevSibling->m_refCount != 0)
			{
				next = node->m_prevSibling;
			}
			else
			{
				next = node->m_parent;
			}
			FreeNode(node);
		}
		node = next;
	}
	*/
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
STATIC bool ProfilerReport::Command_ProfilerReportFrame(EventArgs& args)
{
	uint history = args.GetValue("History", 1);

	ProfilerReportNode* node = gProfileReporter->GetFrameInHistory(history);
	TODO("Handle views and send the recieved node to the required view method");

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
ProfilerReportNode::ProfilerReportNode(ProfilerSample_T* node)
{
	m_allocationCount = node->m_allocCount;
	m_allocationSize = node->m_allocationSizeInBytes;

	m_freeCount = node->m_freeCount;
	m_freedSize = node->m_freeSizeInBytes;

	m_numCalls = 1;

	strcpy_s(m_label, node->m_label);

	m_totalTimeHPC = node->m_endTime - node->m_startTime;
	m_avgTime = m_totalTime;
	m_maxTime = m_totalTime;

	if (node->m_lastChild != nullptr)
	{
		GetChildrenFromSampleRoot(node->m_lastChild, this);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
ProfilerReportNode::~ProfilerReportNode()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerReportNode::GetChildrenFromSampleRoot(ProfilerSample_T* root, ProfilerReportNode* parent)
{
	ProfilerSample_T* node = root;
	parent->m_children.emplace_back(ProfilerReportNode(root));

	ProfilerSample_T* next = nullptr;
	next = root;
	while (next != nullptr)
	{
		if (node->m_prevSibling != nullptr)
		{
			next = node->m_prevSibling;
			GetChildrenFromSampleRoot(next, &parent->m_children.emplace_back(ProfilerReportNode(next)));
		}
		else
		{
			next = nullptr;
		}
	}
}