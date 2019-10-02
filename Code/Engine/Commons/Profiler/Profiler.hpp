#pragma once
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/Profiler/ProfilerSample.hpp"
#include "Engine/Core/EventSystems.hpp"
#include <thread>

//------------------------------------------------------------------------------------------------------------------------------
class Profiler
{
public:
	Profiler();
	~Profiler();

	//------------------------------------------------------------------------------------------------------------------------------
	//Methods
	bool			ProfilerInitialize();
	void			ProfilerShutdown();

	void			ProfilerSetMaxHistoryTime(double seconds);

	void			ProfilerPause();
	void			ProfilerResume();

	void			ProfilerPush(const char* label);
	void			ProfilerPop();

	void			ProfilerUpdate();

	void			ProfilerAllocation(size_t byteSize = 0);
	void			ProfilerFree();

	void			ProfilerBeginFrame(const char* label = "Frame");
	void			ProfilerEndFrame();

	// We can only really 'view' a complete tree
	// these functions return the most recently finished tree
	// use a shared_mutex for accessing trees from the system (as it will try to destroy old ones constantly)
	// `history` is how many frames back we should try to get
	ProfilerSample_T*			ProfilerAcquirePreviousTree(std::thread::id id, uint history = 0);
	ProfilerSample_T*			ProfilerAcquirePreviousTreeForCallingThread(uint history = 0);
	void						ProfilerReleaseTree(ProfilerSample_T* node);

	//------------------------------------------------------------------------------------------------------------------------------
	// Static Methods
	static	Profiler*			CreateInstance();
	static	void				DestroyInstance();
	static	Profiler*			GetInstance();

	static	void				FreeTree(ProfilerSample_T* root);


	static ProfilerSample_T*	AllocateNode();
	static void					FreeNode(ProfilerSample_T* node);

	//------------------------------------------------------------------------------------------------------------------------------
	// Dev Console Events
	static	bool				Command_PauseProfiler(EventArgs& args);
	static	bool				Command_ResumeProfiler(EventArgs& args);
	static	bool				Command_ProfilerReport(EventArgs& args);

private:
	double			m_maxHistoryTime = 60;
	bool			m_isReadyToPause = false;
	bool			m_isReadyToResume = false;

	//------------------------------------------------------------------------------------------------------------------------------
	//Used by Profiler to store nodes with respect to each thread
	static thread_local ProfilerSample_T*	tActiveNode;
	std::vector<ProfilerSample_T*>			m_History;
	std::shared_mutex						m_HistoryLock;

	size_t									m_AllowedSize = 1048576;	//1024 KibiBytes or 1 MebiByte
};

// EXTRA -> more important to the job system for tracking timing across threads
// manually construct a tree without relying on thread_local storage; 
// profile_handle_t ProfilePush( char const* tag, profile_handle_t parent = nullptr ); // attaches, starts, and returns a new node to parent
// profile_handle_t ProfilePop( profile_andle_t active ); // will complete active, and return active's parent (or null if no parent)

//------------------------------------------------------------------------------------------------------------------------------
class ProfilerLogObject
{
public:
	ProfilerLogObject(char const *label)
	{
		gProfiler->ProfilerPush(label);
	}

	~ProfilerLogObject()
	{
		gProfiler->ProfilerPop();
	}
};

//------------------------------------------------------------------------------------------------------------------------------
#define PROFILE_SCOPE( tag )			ProfilerLogObject(tag)
#define PROFILE_FUNCTION()				PROFILE_SCOPE(__FUNCTION__);