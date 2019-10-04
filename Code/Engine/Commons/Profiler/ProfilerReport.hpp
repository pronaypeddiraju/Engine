#pragma once
#include "Engine/Core/EventSystems.hpp"
#include <string>
#include <vector>

typedef unsigned int uint;
struct ProfilerSample_T;

//------------------------------------------------------------------------------------------------------------------------------
class ProfilerReportNode
{
public:
	ProfilerReportNode(ProfilerSample_T* node, ProfilerReportNode* parent = nullptr);
	~ProfilerReportNode();

	void					GetChildrenFromSampleRoot(ProfilerSample_T* root, ProfilerReportNode* parent);

	void					SortByTotalTime();
	void					SortBySelfTime();

	void					GetSelfTime();

	ProfilerReportNode*		m_parent = nullptr;

	uint64_t				m_allocationCount = 0U;
	size_t					m_allocationSize = 0;
	
	uint64_t				m_freeCount = 0U;
	size_t					m_freedSize = 0U;

	uint					m_numCalls = 0U;

	float					m_totalPercent = 0.0f;
	float					m_selfPercent = 0.0f;

	char					m_label[64];

	//Optional:
	uint64_t				m_totalTimeHPC = 0U;
	double					m_totalTime = 0;
	double					m_selfTime = 0;

	double					m_avgTime = 0;	//Avg time per call including children
	double					m_avgSelfTime = 0;	//Avg time per call not including children

	double					m_maxTime = 0;	//longest a certain sample took in the collection
	double					m_maxTimeSelf = 0;	//longest a certain sample took in the collection but without including it's children

	//My kids
	std::vector<ProfilerReportNode>		m_children;
};

//------------------------------------------------------------------------------------------------------------------------------
class ProfilerReport
{
public:
	ProfilerReport();
	~ProfilerReport();

	ProfilerReportNode*		GetFrameInHistory(uint history = 1);
	ProfilerReportNode*		GetRoot();

	static ProfilerReport*	CreateInstance();
	static ProfilerReport*	GetInstance();
	static void				DestroyInstance();

	void					DrawTreeViewAsImGUIWidget(uint history);
	void					DrawFlatViewAsImGUIWidget(uint history);

private:
	void					InitializeReporter();

	void					GenerateTreeFromFrame(ProfilerSample_T* root);
	void					GenerateFlatFromFrame(ProfilerSample_T* root);

	static bool				Command_ProfilerReportFrame(EventArgs& args);

	ProfilerReportNode*		m_root = nullptr;
};
