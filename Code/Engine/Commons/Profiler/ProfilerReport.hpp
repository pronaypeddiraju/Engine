#pragma once
#include "Engine/Core/EventSystems.hpp"
#include <string>
#include <vector>

typedef unsigned int uint;
struct ProfilerSample_T;

enum ProfilerReportMode
{
	TREE_VIEW,
	FLAT_VIEW
};

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

	int						m_allocationCount = 0;
	size_t					m_allocationSize = 0;
	
	int						m_freeCount = 0;
	size_t					m_freedSize = 0;

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

	bool					operator==(const ProfilerReportNode& compare) const;				// Are nodes equal?
};

bool						ComparatorTotalTimeSort(ProfilerReportNode* elementA, ProfilerReportNode* elementB);
bool	 					ComparatorSelfTimeSort(ProfilerReportNode* elementA, ProfilerReportNode* elementB);

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

	void					SetReportMode(ProfilerReportMode mode);
	ProfilerReportMode		GetMode() { return m_activeMode; }
	void					SetRoot(ProfilerReportNode* root);

	//NOTE: This is only for the UI buttons and is to be accessed only by ProfilerReportNode
	int						m_sortTotalTime = 0;
	int						m_sortSelfTime = 0;

private:
	void					InitializeReporter();

	void					GenerateTreeFromFrame(ProfilerSample_T* root);
	void					GenerateFlatFromFrame(ProfilerSample_T* root);
	void					AddToFlatViewVector(ProfilerReportNode& rootNode);

	void					PopulateTreeForImGUI(ProfilerReportNode* root);
	void					PopulateFlatForImGUI();

	static bool				Command_ProfilerToggleMode(EventArgs& args);
	static bool				Command_ProfilerReportFrame(EventArgs& args);

	ProfilerReportNode*		m_root = nullptr;

	uint					m_lastHistoryFrame = 0U;

	ProfilerReportMode		m_activeMode = TREE_VIEW;

	int						m_clicked = 0;

	std::vector<ProfilerReportNode*>	m_flatViewVector;
	void GenerateFlatViewVector(ProfilerReportNode* m_root);
};
