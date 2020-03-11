#ifndef __EVENT_H__
#define __EVENT_H__

#include "Cvar.h"
#include <queue>

#define MAX_KEYS 300

class EventListener;

enum EventType
{
	// APP
	PLAY,
	PAUSE,
	TICK,
	STOP,

	REQUEST_DEFAULT_CONF,
	REQUEST_LOAD,
	REQUEST_SAVE,
	REQUEST_QUIT,

	// Window
	WINDOW_MOVED,
	WINDOW_SIZE_CHANGED,
	WINDOW_HIDE,
	WINDOW_SHOW,
	WINDOW_FOCUS,
	WINDOW_FOCUS_LEAVE,
	WINDOW_QUIT,

	// Renderer
	SET_VSYNC,

	// Gameobjects
	TRANSFORM_MODIFIED,
	PARENT_TRANSFORM_MODIFIED,

	MAX_EVENT_TYPES
};

class Event
{
public:

	Event(EventType t, EventListener* lis, Cvar data = Cvar(), Cvar data2 = Cvar());
	Event(Event& e);
	virtual ~Event();

	static void Push(EventType t, EventListener* lis, Cvar data = Cvar(), Cvar data2 = Cvar());
	static void PumpAll();
	static void Pump();
	static unsigned int RemainingEvents();

	static void ResumeEvents();
	static void PauseEvents();
	static bool isPaused();

private:

	void CallListener() const;
	bool IsValid() const;
	void Clear();

public:

	EventType type;
	Cvar data1;
	Cvar data2;
	const unsigned int timestamp;
	EventListener* listener = nullptr;

private:

	static bool paused;
	static std::queue<Event> events_queue;
};

#endif // __EVENT_H__
