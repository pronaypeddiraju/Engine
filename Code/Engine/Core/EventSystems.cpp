#include "Engine/Core/EventSystems.hpp"

EventSystems* g_eventSystem = nullptr;

EventSystems::EventSystems()
{

}

EventSystems::~EventSystems()
{

}

void EventSystems::StartUp()
{

}

void EventSystems::BeginFrame()
{

}

void EventSystems::EndFrame()
{

}

void EventSystems::ShutDown()
{

}

void EventSystems::SubscribeEventCallBackFn( const std::string& eventName, EventCallBackFn callBack )
{
	//Push the event into the event list
	EventSubscription* subscriber = new EventSubscription(callBack);
	m_eventSubscriptions[eventName].push_back(subscriber);
}

void EventSystems::UnsubscribeEventCallBackFn( const std::string& eventName, EventCallBackFn callBack )
{
	std::map<std::string, SubscribersList >::iterator eventIterator;
	eventIterator = m_eventSubscriptions.begin();
	while(eventIterator != m_eventSubscriptions.end())
	{
		if(eventName.compare(eventIterator->first) == 0)
		{
			for(int subscribeIndex = 0; subscribeIndex < static_cast<int>(eventIterator->second.size()); subscribeIndex++)
			{
				if(eventIterator->second[subscribeIndex]->m_callbackFn == callBack)
				{
					eventIterator->second[subscribeIndex]->m_callbackFn = nullptr;
					eventIterator->second.erase(eventIterator->second.begin() + subscribeIndex);
					break;
				}
			}
			break;
		}

		eventIterator++;
	}
}

int EventSystems::FireEvent( const std::string& eventName )
{
	EventArgs args;
	return FireEvent(eventName, args);
}

int EventSystems::FireEvent( const std::string& eventName, EventArgs& args )
{
	int numFired = 0;
	std::map<std::string, SubscribersList >::iterator eventIterator;

	//Find the function to call
	eventIterator = m_eventSubscriptions.find(eventName);

	//If we are at the end just return 
	if(eventIterator == m_eventSubscriptions.end())
	{
		return 0;
	}

	bool eventConsumption;
	for(int subscriberIndex = 0; subscriberIndex < static_cast<int>(eventIterator->second.size()); subscriberIndex++)
	{
		eventConsumption = eventIterator->second[subscriberIndex]->m_callbackFn(args);
		numFired++;
		if(eventConsumption)
		{
			//The event has been consumed
			break;
		}
	}

	return numFired;
}

int EventSystems::GetNumSubscribersForCommand( const std::string& eventName ) const
{
	//Find the event
	std::map<std::string, SubscribersList >::const_iterator eventIterator;
	eventIterator = m_eventSubscriptions.find(eventName);

	int numEvents = static_cast<int>(eventIterator->second.size());
	return numEvents;
}

void EventSystems::GetSubscribedEventsList( std::vector<std::string>& eventNamesWithSubscribers ) const
{
	std::map<std::string, SubscribersList >::const_iterator eventIterator;
	eventIterator = m_eventSubscriptions.begin();
	while( eventIterator != m_eventSubscriptions.end() )
	{
		if(eventIterator->second.size() != 0)
		{
			eventNamesWithSubscribers.push_back(eventIterator->first);
		}

		eventIterator++;
	}
}
