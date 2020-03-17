#ifndef __MODULE_H__
#define __MODULE_H__

#include "EventListener.h"

class Module : public EventListener
{
public:

	Module(const char* name) : active(false), name(name) {}
	virtual ~Module() {}

	virtual void LoadConfig(bool empty_config) {}
	virtual void SaveConfig() const {}

	virtual bool Init() { return true; }
	virtual bool Start() { return true; }

	virtual bool PreUpdate() { return true; }
	virtual bool Update() { return true; }
	virtual bool PostUpdate() { return true; }

	virtual bool CleanUp() { return true; }

	const char* GetName() const { return name; }

	void SetActive(bool is_active) { active = is_active; }
	bool IsActive() const { return active; }

protected:

	const char*	name;
	bool		active;

};

#endif // __MODULE_H__