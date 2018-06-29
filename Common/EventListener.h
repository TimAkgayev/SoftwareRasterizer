#pragma once
#include "stdinclude.h"

class EventListener
{
public:
	EventListener(EventListener* ObjectThis);
	~EventListener();

	virtual void ProcessEvent(UINT, WPARAM, LPARAM) = 0;

	static void _ProcessEvent(UINT, WPARAM, LPARAM);

private:
	static std::vector<EventListener*> objects;
};

