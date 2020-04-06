#include "Event.h"
#include "EventListener.h"
#include "SDL/include/SDL_timer.h"

bool Event::paused = false;
std::queue<Event> Event::events_queue;

Event::Event(EventType t, EventListener * lis, Cvar d1, Cvar d2)
	: type(t), listener(lis), data1(d1), data2(d2), timestamp(SDL_GetTicks())
{}

Event::Event(const Event& e)
	: type(e.type), listener(e.listener), data1(e.data1), data2(e.data2), timestamp(SDL_GetTicks())
{}

Event::~Event()
{
	Clear();
}

void Event::CallListener() const
{
	if(listener != nullptr) listener->RecieveEvent(*this);
}

bool Event::IsValid() const
{
	return type < MAX_EVENT_TYPES && listener != nullptr;
}

void Event::Push(EventType t, EventListener * lis, Cvar d1, Cvar d2)
{
	if (!Event::paused)
		Event::events_queue.push(Event(t, lis, d1, d2));
}

void Event::Push(EventType t, std::vector<EventListener*>& lis, Cvar d1, Cvar d2)
{
	if (!Event::paused)
		for (std::vector<EventListener*>::iterator it = lis.begin(); it != lis.end(); ++it)
			Event::events_queue.push(Event(t, *it, d1, d2));
}

void Event::Push(const Event e)
{
	if (!Event::paused)
		Event::events_queue.push(e);
}

void Event::PumpAll()
{
	while (!Event::events_queue.empty())
	{
		const Event e = Event::events_queue.front();
		Event::events_queue.pop();

		if (e.IsValid())
			e.CallListener();
	}
}

void Event::Pump()
{
	if (!Event::events_queue.empty())
	{
		const Event e = Event::events_queue.front();
		Event::events_queue.pop();

		if (e.IsValid())
			e.CallListener();
	}
}

unsigned int Event::RemainingEvents()
{
	return Event::events_queue.size();
}

void Event::ResumeEvents()
{
	paused = false;
}

void Event::PauseEvents()
{
	paused = true;
}

bool Event::isPaused()
{
	return paused;
}

void Event::Clear()
{
	type = MAX_EVENT_TYPES;
	listener = nullptr;
}
