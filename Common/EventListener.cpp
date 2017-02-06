#include "EventListener.h"

std::vector<EventListener*> EventListener::objects;

EventListener::EventListener(EventListener* ObjectThis)
{
	objects.push_back(ObjectThis);
}


void EventListener::_ProcessEvent(UINT u, WPARAM w , LPARAM l)
{
	auto vIter = objects.begin();
	for (;vIter != objects.end(); vIter++)
		(*vIter)->ProcessEvent(u, w, l);
}