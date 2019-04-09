//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/NamedStrings.hpp"

typedef bool (*EventCallBackFn)(EventArgs& args);

//------------------------------------------------------------------------------------------------------------------------------
class EventSubscription
{
	friend class EventSystems;

private:
	EventSubscription(EventCallBackFn name)
	{
		m_callbackFn = name;
	}

private:
	EventCallBackFn m_callbackFn = nullptr;
};

//------------------------------------------------------------------------------------------------------------------------------
class EventSystems
{
	//Use this for std::map being stored as m_eventSubscriptions
	typedef std::vector<EventSubscription*> SubscribersList;

public:
	EventSystems();
	~EventSystems();

	void			StartUp();
	void			BeginFrame();
	void			EndFrame();
	void			ShutDown();

	void			SubscribeEventCallBackFn(const std::string& eventName, EventCallBackFn callBack);
	void			UnsubscribeEventCallBackFn(const std::string& eventName, EventCallBackFn callBack);
	int				FireEvent(const std::string& eventName);
	int				FireEvent(const std::string& eventName, EventArgs& args);
	int				GetNumSubscribersForCommand(const std::string& eventName) const;
	void			GetSubscribedEventsList(std::vector<std::string>& eventNamesWithSubscribers) const;

private:
	std::map<std::string, SubscribersList > m_eventSubscriptions;
};