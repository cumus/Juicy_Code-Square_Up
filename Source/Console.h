#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "Module.h"

enum class Commands
{
	NONE,
	VSYNC_ON,
	VSYNC_OFF,
	QUIT,
	FULLSCREEN,
	MAX_FPS,
	PAUSE,
	LOAD,
	SAVE,
	LEVEL_1,
	LEVEL_2,
	GODMODE,

	MAX_CONSOLE_COMMANDS
};


class Console : public Module
{
public:
	Console();
	~Console();

	bool Awake();
	bool Start();
	bool CleanUp();
	bool Update(float dt);

	void Write(const char*);

	Commands ReturnCommands(Commands);

	void Open();
	void Close();

private:
	
	bool opened;
};

#endif // !__CONSOLE_H__
